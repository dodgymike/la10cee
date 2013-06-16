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
	// this makes sure the curPacket is deleted when we're done
	auto_ptr<PacketHeader> curPacketAuto(curPacket);
	
	if(curPacket->packetType() != PACKET_IP) {
		cerr << "NON-IP PACKET" << endl;
		return false;
	}

	if(curPacket->ipPacketType() != IP_TCP) {
		cerr << "\tNON-TCP PACKET" << endl;
		return false;
	}

	m_statMaps->m_totalStats.addStat("ipTotalBytes", curPacket->ipLength());

	// connection tracking
	TCPConnection* curConnection = m_statMaps->m_connections.find(curPacket->sourceIP(), curPacket->sourcePort(), curPacket->destinationIP(), curPacket->destinationPort());			

	// packetSource:
	//		true	== source address on connection maps to packet source address
	//		false	== source address on connection maps to packet destination address
	bool packetSource = true;
	if(curPacket->sourceIP() != curConnection->addresses().ipA()) {
		packetSource = false;
	}

	// connection state
	if(packetSource) {
		curConnection->bytesInIncrement(curPacket->ipLength());
		curConnection->stateSourceSyn(curPacket->syn());
		curConnection->stateSourceAck(curPacket->ack());
		curConnection->stateSourceFin(curPacket->fin());
		curConnection->stateSourceReset(curPacket->rst());
		
		// assume an incoming packet
		curConnection->packetsInIncrement(1);
	} else {
		curConnection->bytesOutIncrement(curPacket->ipLength());
		curConnection->stateDestSyn(curPacket->syn());
		curConnection->stateDestAck(curPacket->ack());
		curConnection->stateDestFin(curPacket->fin());
		curConnection->stateDestReset(curPacket->rst());
			
		// assume an outgoing packet
		curConnection->packetsOutIncrement(1);
	}
			
	// FIXME state machine anyone?
			
	// debug output
	//cerr << "sourceIP (" << sourceIP << ") destinationIP (" << destinationIP << ") sourcePort (" << sourcePort << ") destinationPort (" << destinationPort << ") ";
	stringstream ss;
	ss << "sourceIP (" << curPacket->sourceIP() << ") destinationIP (" << curPacket->destinationIP() << ") sourcePort (" << curPacket->sourcePort() << ") destinationPort (" << curPacket->destinationPort() << ") ";
	// received a reset packet
	if(curConnection->stateSourceReset() || curConnection->stateDestReset()) {
		ss << " RESET" << endl;
		cerr << ss.str();
		curConnection->connectionState(STATE_RESET);

		// store the final close time
		curConnection->closeTime(time(NULL));

		return true;
	}
	
	// one side sent a FIN packet
	if(curConnection->stateSourceFin() || curConnection->stateDestFin()) {
		if(curConnection->connectionState() == STATE_CLOSED) {
			ss << "OOOPS - ALREADY CLOSED " << endl;
		} else if(curConnection->connectionState() == STATE_CLOSED2) {
			ss << " CLOSED" << endl;
			curConnection->connectionState(STATE_CLOSED);
				
			// store the final close time
			curConnection->closeTime(time(NULL));
		} else if(curConnection->connectionState() == STATE_CLOSED1) {
			ss << " CLOSED2" << endl;
			curConnection->connectionState(STATE_CLOSED2);
		} else {
			ss << " CLOSED1" << endl;
			curConnection->connectionState(STATE_CLOSED1);
		}
		cerr << ss.str();
		//cerr << "closed";

		return true;
	} 
	
	// connected
	if(curConnection->stateSourceSyn() && curConnection->stateDestSyn() && curConnection->stateDestAck() && curConnection->stateSourceAck()) {
		//DbgMsg("connected");
		curConnection->connectionState(STATE_CONNECTED);

		return true;
	}
	
	// partially connected
	if(curConnection->stateSourceSyn() || curConnection->stateDestSyn() || curConnection->stateDestAck() || curConnection->stateSourceAck()) {
		if(curConnection->connectionState() == STATE_BAD_CONNECTION_SEQUENCE) {
			// ignore this connection's connect state
			ss << " IGNORING BAD CONNECT STATE" << endl;
		} else if(curConnection->connectionState() == STATE_CONNECTING2) {
			ss << " BAD CONNECTION SEQUENCE" << endl;
			curConnection->connectionState(STATE_BAD_CONNECTION_SEQUENCE);
		} else if(curConnection->connectionState() == STATE_CONNECTING1) {
			ss << " CONNECTING2" << endl;
			curConnection->connectionState(STATE_CONNECTING2);
		} else {
			ss << " CONNECTING1" << endl;
			curConnection->connectionState(STATE_CONNECTING1);
				
			// store the initial connect time
			curConnection->connectTime(time(NULL));
		}
				
		cerr << ss.str();
		//cerr << "connection in progress";

		return true;
	} 
	
	ss << " UNKNOWN" << endl;
	cerr << ss.str();
	//cerr << "unknown";
	curConnection->connectionState(STATE_UNKNOWN);
		
	//cerr << endl;
		
	return false;
}

