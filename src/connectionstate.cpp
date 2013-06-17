#include "connectionstate.h"
#include "tcpconnection.h"

TCPConnectionState* TCPConnectionStateNew::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
	// check for a valid syn flag
	if(!packetHeader.syn()) {
		cerr << "connection state new, first packet does not have syn flag set" << endl;
		return NULL;
	}

	cerr << "!!! connection state new, got syn" << endl;

	tcpConnection.stateSourceSyn(true);
	tcpConnection.synTime(packetHeader.receivedTime());

	return new TCPConnectionStateSyn();
}

TCPConnectionState* TCPConnectionStateSyn::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
	// check that the packet comes from the destination
	if(packetHeader.sourceIP() != tcpConnection.addresses().ipB()) {
		cerr << "connection state syn, second packet is not from the destination ip!" << endl;
		return NULL;
	}

	// check for RST from the destination
	if(packetHeader.rst()) {
		cerr << "connection state syn, rst received from dest!" << endl;
		tcpConnection.stateDestReset(true);
		return new TCPConnectionStateReset();
	}

	// check for a valid syn/ack pair from the destination
	if(!packetHeader.syn() || !packetHeader.ack()) {
		cerr << "connection state syn, second packet has no syn and ack!" << endl;
		return NULL;
	}

	cerr << "!!! connection state new, got syn-ack" << endl;

	tcpConnection.stateDestSyn(true);
	tcpConnection.stateDestAck(true);
	tcpConnection.synAckTime(packetHeader.receivedTime());

	return new TCPConnectionStateSynAck();
}

TCPConnectionState* TCPConnectionStateSynAck::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
	// check that the packet comes from the source
	if(packetHeader.destinationIP() != tcpConnection.addresses().ipB()) {
		cerr << "connection stat synack, third packet is not from the source ip!" << endl;
		return NULL;
	}

	// check for a RST from the source
	if(packetHeader.rst()) {
		cerr << "connection state synack, rst received from source!" << endl;
		return NULL;
	}

	cerr << "!!! connection state new, got final ack" << endl;

	tcpConnection.stateSourceAck(true);
	tcpConnection.ackTime(packetHeader.receivedTime());

	tcpConnection.dumpTimings();

	return new TCPConnectionStateConnected();
}

TCPConnectionState* TCPConnectionStateConnected::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
}

TCPConnectionState* TCPConnectionStateClosed::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
}

TCPConnectionState* TCPConnectionStateReset::nextStep(TCPConnection& tcpConnection, PacketHeader& packetHeader) {
	cerr << "connection state reset, received packet on closed connection!" << endl;
	return NULL;
}


