#include "DBThread.h"

DBThread::DBThread(StatMaps* statMaps, const string& dbConnectionOptions)
	: m_statMaps(statMaps)
{
	m_lastStatOutput = time(NULL);
	m_dbConnection = new connection(dbConnectionOptions);
}

DBThread::~DBThread()
{
	stop();
}

bool DBThread::run()
{
	cerr << "<DBThread>\tStarting" << endl;

	try
	{
	
		// ************** Lock here
		{
			AutoLock lock(&m_runningLock);
			m_running = true;
		}
		// ************** Unlock here
	
		while(true) {
			// stop processing if required
			{
				AutoLock lock(&m_runningLock);
				if(m_running == false) {
					break;
				}
			}
	
			// only output stats every x seconds
			sleep(30);
			
			cerr << "*********************************************************" << endl;
			vector<string>* totalStatsKeys = m_statMaps->m_totalStats.keys();
			auto_ptr<vector<string> > totalStatsKeysAP(totalStatsKeys);
			for(vector<string>::iterator i = totalStatsKeys->begin(); i != totalStatsKeys->end(); i++) {
				cerr << time(NULL) << ":" << *i << "\t:" << m_statMaps->m_totalStats[*i] << endl;
			}

			vector<u_int>* totalPortStatsKeys = m_statMaps->m_totalPortStats.keys();
			auto_ptr<vector<u_int> > totalPortStatsKeysAP(totalPortStatsKeys);
			for(vector<u_int>::iterator i = totalPortStatsKeys->begin(); i != totalPortStatsKeys->end(); i++) {
				cerr << time(NULL) << ":" << *i << "\t:" << m_statMaps->m_totalPortStats[*i] << endl;
			}

			/*
			for(TotalStatsByIp_t::iterator i = m_totalStatsByIp.begin(); i != m_totalStatsByIp.end(); i++) {
				cerr << time(NULL) << ":" << i->first << "\t:" << i->second << endl;
			}
			*/

			/*
			for(ConnectionStats_t::iterator i = m_connectionStats.begin(); i != m_connectionStats.end(); i++) {
				TCPConnection* curConnection = i->second;
			
				cerr << time(NULL) << ":" 
					<< curConnection->sourceIP() << ":" << curConnection->sourcePort()
					<< ":"
					<< curConnection->destinationIP() << ":" << curConnection->destinationPort()
					<< ":"
					<< curConnection->bytesIn()
					<< ":"
					<< curConnection->bytesOut()
					<< endl;
			}
			*/
			cerr << "----------------------------------------------------------" << endl;
			vector<IPTCPPort>* connectionsKeys = m_statMaps->m_connections.keys();
			auto_ptr<vector<IPTCPPort> > connectionsKeysAP(connectionsKeys);
			for(vector<IPTCPPort>::iterator portIterator = connectionsKeys->begin(); portIterator != connectionsKeys->end(); portIterator++) {
				IPTCPPort ipPort = *portIterator;
				TCPConnection* curConnection = m_statMaps->m_connections.find(ipPort);
			
				stringstream ss;
				ss << ipPort.ipA() << ":" << ipPort.portA() << ":" << ipPort.ipB() << ":" << ipPort.portB()
					<< ":" << curConnection->bytesIn() << ":" << curConnection->bytesOut()
					<< ":" << curConnection->connectionState()
				;
					
				cerr << ss.str() << endl;
			}
			cerr << "*********************************************************" << endl;
			
			cerr << "1.1" << endl;
			StatsTransactor st(m_statMaps);
			cerr << "1.1.1:m_dbConnection (" << m_dbConnection << ")" << endl;
			cerr << "1.2:m_dbConnection is open (" << m_dbConnection->is_open() << ")" << endl;
			//m_dbConnection->activate();
			cerr << "1.3:m_dbConnection is open (" << m_dbConnection->is_open() << ")" << endl;
			m_dbConnection->perform(st);
			cerr << "1.4" << endl;
		}
	}
	catch(Exception& e)
	{
		cerr << "location [" << e.getLocation() << "] error [" << e.getError() << "]" << endl;
//		abort();
	}


	cerr << "<DBThread>\tprocessing complete" << endl;
	return(true);
}


StatsTransactor::StatsTransactor(StatMaps* statMaps)
	: m_statMaps(statMaps)
{
	
}

void StatsTransactor::operator()(work &work) {
	vector<IPTCPPort>* connectionsKeys = m_statMaps->m_connections.keys();
	auto_ptr<vector<IPTCPPort> > connectionsKeysAP(connectionsKeys);
	for(vector<IPTCPPort>::iterator portIterator = connectionsKeys->begin(); portIterator != connectionsKeys->end(); portIterator++) {
		IPTCPPort ipPort = *portIterator;
		TCPConnection* curConnection = m_statMaps->m_connections.find(ipPort);

		stringstream ss;
		ss 
			<< "insert into connection_pair ("
			<< "ipA, portA, ipB, portB, start_date, end_date, state, data_in, data_out, packets_in, packets_out) "
			<< "values ("
			<< "'" << ipPort.ipA() << "'" 
			<< ", " << ipPort.portA() 
			<< ", " << "'" <<  ipPort.ipB() << "'"
			<< ", " << ipPort.portB()
			<< ", " << "CURRENT_TIMESTAMP"
			<< ", " << "CURRENT_TIMESTAMP"
			<< ", " << curConnection->connectionState()
			<< ", " << curConnection->bytesIn()
			<< ", " << curConnection->bytesOut()
			<< ", " << curConnection->packetsIn()
			<< ", " << curConnection->packetsOut()
			<< ")"
		;
		
		cerr << "executing SQL (" << ss.str() << ")" << endl;
		
		work.exec(ss.str());
		
		if((curConnection->connectionState() == STATE_CLOSED) || (curConnection->connectionState() == STATE_RESET)) {
			m_closedConnections.push_back(ipPort);
		}
	}
}

void StatsTransactor::on_commit() {
	cerr << "(StatsTransactor::on_commit)-*** CALLED ***" << endl;
	for(vector<IPTCPPort>::iterator i = m_closedConnections.begin(); i != m_closedConnections.end(); i++) {
		m_statMaps->m_connections.remove(*i);
	}
}

/*
create table connection_pair (
	id serial not null primary key
	,ipA text not null
	,portA int not null
	,ipB text not null
	,portB int not null
	,start_date timestamp not null
	,end_date timestamp not null
	,state text not null
	,data_in int not null default 0
	,data_out int not null default 0
	,packets_in int not null default 0
	,packets_out int not null default 0
);
*/

