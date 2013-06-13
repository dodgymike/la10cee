#include "tcpconnection.h"

using namespace std;

TCPConnection::TCPConnection(const string& srcIp, const u_int srcPort, const string& destIp, const u_int destPort)
		: m_addresses(srcIp, srcPort, destIp, destPort)
	{
	m_bytesIn	= 0;
	m_bytesOut	= 0;
	m_packetsIn	= 0;
	m_packetsOut	= 0;
	
	m_connectTime	= 0;
	m_closeTime		= 0;
	
	m_stateSourceSyn	= false;
	m_stateSourceAck	= false;
	m_stateSourceReset	= false;
	m_stateSourceFin	= false;
	m_stateDestSyn		= false;
	m_stateDestAck		= false;
	m_stateDestReset	= false;
	m_stateDestFin		= false;
	
	m_connectionState = STATE_NEW;
}
	
TCPConnection::TCPConnection(const TCPConnection& rhs)
		: m_addresses(rhs.m_addresses)
{
	m_bytesIn = rhs.m_bytesIn;
	m_bytesOut = rhs.m_bytesOut;
	m_packetsIn = rhs.m_packetsIn;
	m_packetsOut = rhs.m_packetsOut;
	
	m_connectTime	= rhs.m_connectTime;
	m_closeTime		= rhs.m_closeTime;
	
	m_stateSourceSyn		= rhs.m_stateSourceSyn;
	m_stateSourceAck		= rhs.m_stateSourceAck;
	m_stateSourceReset		= rhs.m_stateSourceReset;
	m_stateSourceFin		= rhs.m_stateSourceFin;
	m_stateDestSyn			= rhs.m_stateDestSyn;
	m_stateDestAck			= rhs.m_stateDestAck;
	m_stateDestReset		= rhs.m_stateDestReset;
	m_stateDestFin			= rhs.m_stateDestFin;
	
	m_connectionState		= rhs.m_connectionState;
}
	
const IPTCPPort& TCPConnection::addresses() {
	AutoLock autoLock(&m_mutex);
	
	return m_addresses;
}

u_long TCPConnection::bytesIn() {
	AutoLock autoLock(&m_mutex);
	
	return m_bytesIn;
}
u_long TCPConnection::bytesIn(u_long newBytesIn) {
	AutoLock autoLock(&m_mutex);
	
	return(m_bytesIn = newBytesIn);
}
u_long TCPConnection::bytesInIncrement(u_long newBytesIn) {
	AutoLock autoLock(&m_mutex);
	
	return(m_bytesIn += newBytesIn);
}

u_long TCPConnection::bytesOut() {
	AutoLock autoLock(&m_mutex);
	
	return m_bytesOut;
}
u_long TCPConnection::bytesOut(u_long newBytesOut) {
	AutoLock autoLock(&m_mutex);
	
	return(m_bytesOut = newBytesOut);
}
u_long TCPConnection::bytesOutIncrement(u_long newBytesOut) {
	AutoLock autoLock(&m_mutex);
	
	return(m_bytesOut += newBytesOut);
}

u_long TCPConnection::packetsIn() {
	AutoLock autoLock(&m_mutex);
	
	return m_packetsIn;
}
u_long TCPConnection::packetsIn(u_long newPacketsIn) {
	AutoLock autoLock(&m_mutex);
	
	return(m_packetsIn = newPacketsIn);
}
u_long TCPConnection::packetsInIncrement(u_long newPacketsIn) {
	AutoLock autoLock(&m_mutex);
	
	return(m_packetsIn += newPacketsIn);
}

u_long TCPConnection::packetsOut() {
	AutoLock autoLock(&m_mutex);
	
	return m_packetsOut;
}
u_long TCPConnection::packetsOut(u_long newPacketsOut) {
	AutoLock autoLock(&m_mutex);
	
	return(m_packetsOut = newPacketsOut);
}
u_long TCPConnection::packetsOutIncrement(u_long newPacketsOut) {
	AutoLock autoLock(&m_mutex);
	
	return(m_packetsOut += newPacketsOut);
}

bool TCPConnection::stateSourceSyn() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateSourceSyn;
}
bool TCPConnection::stateSourceAck() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateSourceAck;
}
bool TCPConnection::stateSourceReset() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateSourceReset;
}
bool TCPConnection::stateSourceFin() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateSourceFin;
}
bool TCPConnection::stateDestSyn() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateDestSyn;
}
bool TCPConnection::stateDestAck() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateDestAck;
}
bool TCPConnection::stateDestReset() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateDestReset;
}
bool TCPConnection::stateDestFin() {
	AutoLock autoLock(&m_mutex);
	
	return m_stateDestFin;
}

bool TCPConnection::stateSourceSyn(bool syn) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateSourceSyn = syn);
}
bool TCPConnection::stateSourceAck(bool ack) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateSourceAck = ack);
}
bool TCPConnection::stateSourceReset(bool reset) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateSourceReset = reset);
}
bool TCPConnection::stateSourceFin(bool fin) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateSourceFin = fin);
}
bool TCPConnection::stateDestSyn(bool syn) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateDestSyn = syn);
}
bool TCPConnection::stateDestAck(bool ack) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateDestAck = ack);
}
bool TCPConnection::stateDestReset(bool reset) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateDestReset = reset);
}
bool TCPConnection::stateDestFin(bool fin) {
	AutoLock autoLock(&m_mutex);
	
	return (m_stateDestFin = fin);
}

ConnectionState TCPConnection::connectionState() {
	AutoLock autoLock(&m_mutex);
	
	return m_connectionState;
}
ConnectionState TCPConnection::connectionState(ConnectionState state) {
	AutoLock autoLock(&m_mutex);
	
	return (m_connectionState = state);
}

long TCPConnection::connectTime() {
	AutoLock autoLock(&m_mutex);
	
	return m_connectTime;
}
long TCPConnection::TCPConnection::connectTime(long newConnectTime) {
	AutoLock autoLock(&m_mutex);
	
	return (m_connectTime = newConnectTime);
}

long TCPConnection::closeTime() {
	AutoLock autoLock(&m_mutex);
	
	return m_closeTime;
}
long TCPConnection::closeTime(long newCloseTime) {
	AutoLock autoLock(&m_mutex);
	
	return (m_closeTime = newCloseTime);
}


//	int operator<(const IPTCPPort& rhs) const;

