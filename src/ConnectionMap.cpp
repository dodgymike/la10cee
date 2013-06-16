#include "ConnectionMap.h"

ConnectionMap::ConnectionMap()
{
}

ConnectionMap::~ConnectionMap()
{
}

TCPConnection* ConnectionMap::find(const string& ipA, int portA, const string& ipB, int portB) {
	AutoLock autoLock(&m_mutex);
	
	TCPConnection* curConnection;
	
	// packetSource:
	//		true	== source address on connection maps to packet source address
	//		false	== source address on connection maps to packet destination address
	bool packetSource = true;
	if(m_connectionMap.find(IPTCPPort(ipA, portA, ipB, portB)) != m_connectionMap.end()) {
		curConnection = m_connectionMap[IPTCPPort(ipA, portA, ipB, portB)];
	} else if(m_connectionMap.find(IPTCPPort(ipB, portB, ipA, portA)) != m_connectionMap.end()) {
		curConnection = m_connectionMap[IPTCPPort(ipB, portB, ipA, portA)];
		
		// opposite direction from the original packet
		packetSource = false;
	} else {
		stringstream ss;
		ss << "new connection pair for ";
		ss << " source ip (" << ipA << ") ";
		ss << " source port (" << portA << ")";
		ss << " dest ip (" << ipB << ")";
		ss << " dest port (" << portB << ")"; 
		ss << endl;
		cerr << ss.str();
		curConnection = new TCPConnection(ipA, portA, ipB, portB);

		m_connectionMap[IPTCPPort(ipA, portA, ipB, portB)] = curConnection;
	}

	return curConnection;
}

TCPConnection* ConnectionMap::find(IPTCPPort ipPort) {
	return find(ipPort.ipA(), ipPort.portA(), ipPort.ipB(), ipPort.portB());
}


vector<IPTCPPort>* ConnectionMap::keys() {
	AutoLock lock(&m_mutex);
	
	vector<IPTCPPort>* keys = new vector<IPTCPPort>();
	
	for(map<IPTCPPort, TCPConnection*>::iterator i = m_connectionMap.begin(); i != m_connectionMap.end(); i++) {
		keys->push_back(i->first);
	}
	
	return keys;
}

void ConnectionMap::remove(IPTCPPort ipPort) {
	AutoLock lock(&m_mutex);
	
	if(m_connectionMap.erase(ipPort) <= 0) {
		cerr << "***OOOPS*** Couldn't erase ipPort ipA (" << ipPort.ipA() << ") portA (" << ipPort.portA() << ") ipB (" << ipPort.portB() << ") portB (" << ipPort.portB() << ")" << endl;
	} else {
		cerr << "erasing ipPort ipA (" << ipPort.ipA() << ") portA (" << ipPort.portA() << ") ipB (" << ipPort.portB() << ") portB (" << ipPort.portB() << ")" << endl;
	}
}
