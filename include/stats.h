#ifndef STATS_H_
#define STATS_H_

#include "packet_data.h"
#include "thread.h"
#include "lockedstatsmap.h"

#include <map>
#include <iostream>
#include <sstream>


/*
#include "iptcpport.h"
#include "tcpconnection.h"
#include "exception.h"
#include "ConnectionMap.h"
*/

typedef LockedStatsMap<string, u_long> TotalStats_t;
typedef LockedStatsMap<u_int, u_long> TotalPortStats_t;
typedef LockedStatsMap<string, u_long> TotalStatsByIp_t;

class StatMaps {
public:
	TotalStats_t m_totalStats;
	TotalPortStats_t m_totalPortStats;
	TotalStatsByIp_t m_totalStatsByIp;
	ConnectionMap m_connections;
};

#endif /*STATS_H_*/
