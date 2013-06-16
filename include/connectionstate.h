#ifndef CONNECTION_STATE_H
#define CONNECTION_STATE_H

enum ConnectionState { 
	STATE_NEW = 0, 
	STATE_CONNECTING1, 
	STATE_CONNECTING2, 
	STATE_BAD_CONNECTION_SEQUENCE, 
	STATE_CONNECTED, 
	STATE_CLOSING, 
	STATE_RESET, 
	STATE_CLOSED1, 
	STATE_CLOSED2, 
	STATE_CLOSED, 
	STATE_UNKNOWN 
}; 

/*
class TCPConnectionState {
public:
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) = 0;
};

class TCPConnectionStateSyn {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateSynAck {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateAck {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateConnected {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};

class TCPConnectionStateClosed {
	TCPConnectionState* nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader);
};
*/

//std::ostream& operator<< (std::ostream & os, ConnectionState state);

#endif

