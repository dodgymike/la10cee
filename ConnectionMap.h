#ifndef CONNECTIONMAP_H_
#define CONNECTIONMAP_H_

#include <map>
#include <vector>
#include <sstream>

#include "threadhelpers.h"
#include "iptcpport.h"
#include "tcpconnection.h"

using namespace std;

class ConnectionMap
{
public:
	ConnectionMap();
	virtual ~ConnectionMap();
	
	TCPConnection* find(const string& ipA, int portA, const string& ipB, int portB);
	TCPConnection* find(IPTCPPort ipPort);
	
	void remove(IPTCPPort ipPort);
	
	vector<IPTCPPort>* keys();
private:
	map<IPTCPPort, TCPConnection*> m_connectionMap;
	
	resourceMutexRetryLock m_mutex;
};

#endif /*CONNECTIONMAP_H_*/
