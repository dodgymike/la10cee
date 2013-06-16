#include "iptcpport.h"

const int IPTCPPort::operator<(const IPTCPPort& rhs) const {
	bool ipAEquals = (ipA() == rhs.ipA());
	bool ipBEquals = (ipB() == rhs.ipB());
	bool portAEquals = (portA() == rhs.portA());
	bool portBEquals = (portB() == rhs.portB());
	
	if(ipAEquals && ipBEquals && portAEquals && portBEquals) {
		return 0;
	} else if(ipAEquals && ipBEquals && portAEquals) {
		return portB() < rhs.portB();
	} else if(ipAEquals && ipBEquals) {
		return portA() < rhs.portA();
	} else if(ipAEquals) {
		return ipB() < rhs.ipB();
	} else {
		return ipA() < rhs.ipA();
	}
}

