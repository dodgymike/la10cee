#include "PacketDecodingThread.h"

PacketDecodingThread::PacketDecodingThread(PacketHandlerData* packetData, StatMaps* statMaps)
	: m_packetData(packetData), m_statMaps(statMaps)
{
	m_lastStatOutput = time(NULL);
}

PacketDecodingThread::~PacketDecodingThread()
{
	stop();
}

/*
bool PacketDecodingThread::stop()
{
	AutoLock lock(&m_runningLock);
	m_running = false;

	return true;
}
*/

bool PacketDecodingThread::run()
{
	cerr << "<PacketDecodingThread>\tStarting" << endl;

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


	cerr << "<PacketDecodingThread>\tprocessing complete" << endl;
	return(true);
}

bool PacketDecodingThread::_updateStats(PacketHeader* curPacket) {
	// this makes sure the curPacket is deleted when we're done
	auto_ptr<PacketHeader> curPacketAuto(curPacket);
	
	if(curPacket->packetType() == PACKET_IP) {
		m_statMaps->m_totalStats.addStat("ipTotalBytes", curPacket->ipLength());

		if(curPacket->ipPacketType() == IP_TCP) {
			m_statMaps->m_totalPortStats.addStat(curPacket->sourcePort(), curPacket->ipLength());
			m_statMaps->m_totalPortStats.addStat(curPacket->destinationPort(), curPacket->ipLength());
			
			m_statMaps->m_totalStatsByIp.addStat(curPacket->sourceIP(), curPacket->ipLength());
			m_statMaps->m_totalStatsByIp.addStat(curPacket->destinationIP(), curPacket->ipLength());
			
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
				if(curPacket->syn()) {
					curConnection->stateSourceSyn(true);
				}
				if(curPacket->ack()) {
					curConnection->stateSourceAck(true);
				}
				if(curPacket->fin()) {
					curConnection->stateSourceFin(true);
				}
				if(curPacket->rst()) {
					curConnection->stateSourceReset(true);
				}
				
				// assume an incoming packet
				curConnection->packetsInIncrement(1);
			} else {
				curConnection->bytesOutIncrement(curPacket->ipLength());
				if(curPacket->syn()) {
					curConnection->stateDestSyn(true);
				}
				if(curPacket->ack()) {
					curConnection->stateDestAck(true);
				}
				if(curPacket->fin()) {
					curConnection->stateDestFin(true);
				}
				if(curPacket->rst()) {
					curConnection->stateDestReset(true);
				}
				
				// assume an outgoing packet
				curConnection->packetsOutIncrement(1);
			}
			
			// FIXME state machine anyone?
			
			// debug output
			//cerr << "sourceIP (" << sourceIP << ") destinationIP (" << destinationIP << ") sourcePort (" << sourcePort << ") destinationPort (" << destinationPort << ") ";
			stringstream ss;
			ss << "sourceIP (" << curPacket->sourceIP() << ") destinationIP (" << curPacket->destinationIP() << ") sourcePort (" << curPacket->sourcePort() << ") destinationPort (" << curPacket->destinationPort() << ") ";
			/*
			if(curConnection->stateSourceReset() || curConnection->stateDestReset()) {
				ss << " RESET" << endl;
				cerr << ss.str();
				curConnection->connectionState(STATE_RESET);

				// store the final close time
				curConnection->closeTime(time(NULL));
			} else if(curConnection->stateSourceFin() || curConnection->stateDestFin()) {
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
			} else if(curConnection->stateSourceSyn() && curConnection->stateDestSyn() && curConnection->stateDestAck() && curConnection->stateSourceAck()) {
				//DbgMsg("connected");
				curConnection->connectionState(STATE_CONNECTED);
			} else if(curConnection->stateSourceSyn() || curConnection->stateDestSyn() || curConnection->stateDestAck() || curConnection->stateSourceAck()) {
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
			} else {
				ss << " UNKNOWN" << endl;
				cerr << ss.str();
				//cerr << "unknown";
				curConnection->connectionState(STATE_UNKNOWN);
			}
			*/
			
			//cerr << endl;
		} else if(curPacket->ipPacketType() == IP_UDP) {
			stringstream ss;
			ss << "UDP-src-" << curPacket->sourceIP() << "-dst-" << curPacket->destinationIP() << "-ip_len-" << curPacket->ipLength() << endl;
			cerr << ss.str();
		} else if(curPacket->ipPacketType() == IP_ICMP) {
			stringstream ss;
			ss << "ICMP-src-" << curPacket->sourceIP() << "-dst-" << curPacket->destinationIP() << "-ip_len-" << curPacket->ipLength() << endl;
			cerr << ss.str();
		} else {
			stringstream ss;
			ss << "UNKNOWN-src-" << curPacket->sourceIP() << "-dst-" << curPacket->destinationIP() << "-ip_len-" << curPacket->ipLength() << endl;
			cerr << ss.str();
		}
		
	} else  if (curPacket->packetType() == PACKET_ARP) {
		cerr << "ARP-" << endl;
	} else {
		cerr << "UNKNOWN" << endl;
	}
	
	//cerr << endl;
	
	/* THANK YOU RICHARD STEVENS!!! RIP*/
	/*
	u_char *ptr = eptr->ether_dhost;
	int i = ETHER_ADDR_LEN;
	fprintf(stderr, "Dest/Source MAC-");
	do{
		//fprintf(stderr, "%s%.2x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
		fprintf(stderr, "%s%.2x",":",*ptr++);
	} while(--i>0);
	fprintf(stderr, "-");
	
	ptr = eptr->ether_shost;
	i = ETHER_ADDR_LEN;
	do{
		fprintf(stderr, "%s%.2x",":",*ptr++);
	} while(--i>0);
	fprintf(stderr, "\n");
	*/

	return true;
}


