#ifndef DBTHREAD_H
#define DBTHREAD_H

#include "packet_data.h"
#include "thread.h"
#include "lockedstatsmap.h"

#include <map>
#include <iostream>
#include <sstream>

#include <connection>
#include <transactor>

#include "connectionstate.h"
#include "iptcpport.h"
#include "tcpconnection.h"
#include "exception.h"
#include "ConnectionMap.h"

using namespace std;

using namespace pqxx;


class DBThread : public Thread
{
public:
	DBThread(StatMaps* statMaps, const string& dbConnectionOptions);
	~DBThread();

	virtual bool run();
	//virtual bool stop();
	
	bool storeMaps();

private:
	time_t m_lastStatOutput;
	StatMaps* m_statMaps;
	
	connection* m_dbConnection;
};

class StatsTransactor : public transactor<work>
{
public:
	StatsTransactor(StatMaps* statMaps);
	void operator()(work &work);
	void on_commit();
private:
	StatMaps* m_statMaps;
	vector<IPTCPPort> m_closedConnections;
};

#endif

