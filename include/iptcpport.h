#ifndef IPTCPPORT_H
#define IPTCPPORT_H

#include <string>
#include <sys/types.h>

using namespace std;

class IPTCPPort
{
public:
	IPTCPPort(const string& ipA, const u_int portA, const string& ipB, const u_int portB)
		: m_ipA(ipA), m_portA(portA), m_ipB(ipB), m_portB(portB)
	{
	}
	
	IPTCPPort(const IPTCPPort& rhs)
	{
		m_ipA = rhs.m_ipA;
		m_portA = rhs.m_portA;
		m_ipB = rhs.m_ipB;
		m_portB = rhs.m_portB;
	}
	
	const string ipA() const {
		return m_ipA;
	}
	
	const u_int portA() const {
		return m_portA;
	}
	
	const string ipB() const {
		return m_ipB;
	}
	
	const u_int portB() const {
		return m_portB;
	}
	
	const int operator<(const IPTCPPort& rhs) const;
private:
	string m_ipA;
	u_int m_portA;
	string m_ipB;
	u_int m_portB;
};

#endif

