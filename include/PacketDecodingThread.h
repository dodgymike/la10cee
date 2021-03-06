#ifndef PACKETDECODINGTHREAD_H
#define PACKETDECODINGTHREAD_H

#include "packet_data.h"
#include "thread.h"
#include "lockedstatsmap.h"

#include <memory>
#include <map>
#include <iostream>
#include <sstream>


#include "connectionstate.h"
#include "iptcpport.h"
#include "tcpconnection.h"
#include "exception.h"
#include "ConnectionMap.h"

#include "stats.h"

using namespace std;


class PacketDecodingThread : public Thread
{
public:
	PacketDecodingThread(PacketHandlerData* packetData, StatMaps* statMaps);
	~PacketDecodingThread();

	virtual bool run();
	//virtual bool stop();

private:
	bool _updateStats(PacketHeader* curPacket);

	time_t m_lastStatOutput;

	PacketHandlerData* m_packetData;
	StatMaps* m_statMaps;
};

#endif

