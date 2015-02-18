#ifndef PACKETDATA_H
#define PACKETDATA_H

#define MAX_PACKET_CAPTURE_SIZE 4096
#define SIZE_ETHERNET 14

#include <vector>
#include <pcap.h>

#include "platform.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> /* includes net/ethernet.h */
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <time.h>

#include <string.h>
//#include "stats.h"
#include "object_queue.h"

using namespace std;

enum PacketType {
	PACKET_IP,
	PACKET_ARP,
	PACKET_UNKNOWN
};

enum IPPacketType {
	IP_TCP,
	IP_UDP,
	IP_ICMP,
	IP_UNKNOWN
};

#define SIZE_ETH_IP_HEADER (SIZE_ETHERNET + sizeof(ip))
#define SIZE_ETH_IP_TCP_HEADER (SIZE_ETH_IP_HEADER + sizeof(tcphdr))

class PacketHeader {
public:
	PacketHeader(const pcap_pkthdr* header, const u_char* data, timespec receivedTime);
	virtual ~PacketHeader();
	
	timespec receivedTime();
	PacketType packetType();
	IPPacketType ipPacketType();
	u_short ipLength();
	string sourceMAC();
	string destinationMAC();
	string sourceIP();
	string destinationIP();
	u_int16_t sourcePort();
	u_int16_t destinationPort();
	bool fin();
	bool syn();
	bool rst();
	bool psh();
	bool ack();
private:
	PacketType m_packetType;
	IPPacketType m_ipPacketType;
	
	u_short m_ipLength;

	string m_sourceMAC;
	string m_destinationMAC;
	string m_sourceIP;
	string m_destinationIP;
	
	u_int16_t m_sourcePort;
	u_int16_t m_destinationPort;

	timespec m_receivedTime;
	
	bool m_fin;
	bool m_syn;
	bool m_rst;
	bool m_psh;
	bool m_ack;
	
	bool m_validPacket;
	
	bool _decodePacket(const pcap_pkthdr* header, const u_char* packetData);
};

struct PacketHandlerData {
	ObjectQueue<PacketHeader*> packets;
};

#endif

