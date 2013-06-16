#ifndef CONNECTION_STATE_H
#define CONNECTION_STATE_H

#include "packet_data.h"

using namespace std;

class TCPConnection;

class TCPConnectionState {
public:
	virtual TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) = 0;
};

class TCPConnectionStateNew : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateSyn : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateSynAck : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateAck : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateConnected : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateClosed : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateReset : public TCPConnectionState {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

#endif

