#include "LatencyMonitoringPacketDecodingThread.h"

LatencyMonitoringPacketDecodingThread::LatencyMonitoringPacketDecodingThread(PacketHandlerData* packetData, StatMaps* statMaps)
	: m_packetData(packetData), m_statMaps(statMaps)
{
	m_lastStatOutput = time(NULL);
}

LatencyMonitoringPacketDecodingThread::~LatencyMonitoringPacketDecodingThread()
{
	stop();
}

/*
bool LatencyMonitoringPacketDecodingThread::stop()
{
	AutoLock lock(&m_runningLock);
	m_running = false;

	return true;
}
*/

bool LatencyMonitoringPacketDecodingThread::run()
{
	cerr << "<LatencyMonitoringPacketDecodingThread>\tStarting" << endl;

	try
	{

	// ************** Lock here
	{
		AutoLock lock(&m_runningLock);
		m_running = true;
	}
	// ************** Unlock here

		PacketHeader* curPacket;
		u_long packetCount = 0;
		
		while(!m_packetData->packets.producerComplete()) {
			// stop processing if required
			{
				AutoLock lock(&m_runningLock);
				if(m_running == false) {
					break;
				}
			}

			if(!m_packetData->packets.pop(curPacket)) {
				sleep(1);
			} else {
				_updateStats(curPacket);
				packetCount++;
			}
		}
	}
	catch(Exception& e)
	{
		cerr << "location [" << e.getLocation() << "] error [" << e.getError() << "]" << endl;
//		abort();
	}


	cerr << "<LatencyMonitoringPacketDecodingThread>\tprocessing complete" << endl;
	return(true);
}

bool LatencyMonitoringPacketDecodingThread::_updateStats(PacketHeader* curPacket) {
//	cerr << "<LatencyMonitorngPacketDecodingThread::_updateStats>\treceived a packet" << endl;

	// this makes sure the curPacket is deleted when we're done
	auto_ptr<PacketHeader> curPacketAuto(curPacket);
	
	if(curPacket->packetType() != PACKET_IP) {
//		cerr << "NON-IP PACKET" << endl;
		return false;
	}

	if(curPacket->ipPacketType() != IP_TCP) {
//		cerr << "\tNON-TCP PACKET" << endl;
		return false;
	}

	m_statMaps->m_totalStats.addStat("ipTotalBytes", curPacket->ipLength());

	stringstream ss;
	ss << "sourceIP (" << curPacket->sourceIP() << ") destinationIP (" << curPacket->destinationIP() << ") sourcePort (" << curPacket->sourcePort() << ") destinationPort (" << curPacket->destinationPort() << ") ";
		
	// connection tracking
	TCPConnection* curConnection = m_statMaps->m_connections.find(curPacket->sourceIP(), curPacket->sourcePort(), curPacket->destinationIP(), curPacket->destinationPort());			
	if(!curConnection->handlePacket(curPacket)) {
		cerr << "ignoring packet" << endl;
		return false;
	}

	return true;
}

