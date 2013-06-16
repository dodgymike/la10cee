#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <time.h>

#include "iptcpport.h"
#include "threadhelpers.h"
#include "packet_data.h"
#include "connectionstate.h"

using namespace std;

class TCPConnection
{
public:
	TCPConnection(const string& srcIp, const u_int srcPort, const string& destIp, const u_int destPort);
	TCPConnection(const TCPConnection& rhs);

	//void setConnectionDetails(const string& srcIp, const u_int srcPort, const string& destIp, const u_int destPort);
	bool handlePacket(PacketHeader* curPacket);

	const IPTCPPort& addresses();

	u_long bytesIn();
	u_long bytesIn(u_long newBytesIn);
	u_long bytesInIncrement(u_long newBytesIn);
	
	u_long bytesOut();
	u_long bytesOut(u_long newBytesOut);
	u_long bytesOutIncrement(u_long newBytesOut);

	u_long packetsIn();
	u_long packetsIn(u_long newPacketsIn);
	u_long packetsInIncrement(u_long newPacketsIn);
	
	u_long packetsOut();
	u_long packetsOut(u_long newPacketsOut);
	u_long packetsOutIncrement(u_long newPacketsOut);

	timespec synTime();
	timespec synTime(timespec synTime);

	timespec synAckTime();
	timespec synAckTime(timespec synAckTime);

	timespec ackTime();
	timespec ackTime(timespec ackTime);
	
	long connectTime();
	long connectTime(long newConnectTime);
	
	long closeTime();
	long closeTime(long newCloseTime);
	
	TCPConnectionState* connectionState();
	bool stateSourceSyn();
	bool stateSourceAck();
	bool stateSourceReset();
	bool stateSourceFin();
	bool stateDestSyn();
	bool stateDestAck();
	bool stateDestReset();
	bool stateDestFin();

	bool stateSourceSyn(bool syn);
	bool stateSourceAck(bool ack);
	bool stateSourceReset(bool reset);
	bool stateSourceFin(bool fin);
	bool stateDestSyn(bool syn);
	bool stateDestAck(bool ack);
	bool stateDestReset(bool reset);
	bool stateDestFin(bool fin);
	
	int operator<(const IPTCPPort& rhs) const;
private:
	IPTCPPort m_addresses;
	u_long m_bytesIn;
	u_long m_bytesOut;
	u_long m_packetsIn;
	u_long m_packetsOut;

	// state
	TCPConnectionState* m_connectionState;

	bool m_stateSourceSyn;
	bool m_stateSourceAck;
	bool m_stateSourceReset;
	bool m_stateSourceFin;
	bool m_stateDestSyn;
	bool m_stateDestAck;
	bool m_stateDestReset;
	bool m_stateDestFin;
	
	// three-way handshake timings
	timespec m_synTime;
	timespec m_synAckTime;
	timespec m_ackTime;

	// connect and close times
	long m_connectTime;
	long m_closeTime;
	
	resourceMutexRetryLock m_mutex;
};

#endif


