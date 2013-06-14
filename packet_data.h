#ifndef PACKETDATA_H
#define PACKETDATA_H

#define MAX_PACKET_CAPTURE_SIZE 4096
#define SIZE_ETHERNET 14

#include <vector>
#include <pcap.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> /* includes net/ethernet.h */
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "stats.h"
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
	PacketHeader(const pcap_pkthdr* header, const u_char* data, time_t p_seconds, long p_nanoseconds) {
		// default to a bad packet
		m_validPacket = false;
		m_packetType = PACKET_UNKNOWN;
		m_ipPacketType = IP_UNKNOWN;
		
		m_sourceMAC = "UNKNOWN";
		m_destinationMAC = "UNKNOWN";
		m_sourceIP = "UNKNOWN";
		m_destinationIP = "UNKNOWN";
		
		m_sourcePort = -1;
		m_destinationPort = -1;

		m_pSeconds = p_seconds;
		m_pNanoSeconds = p_nanoseconds;
		
		_decodePacket(header, data);
	}
	
	~PacketHeader() {
	}
	
	time_t pSeconds() {
		return m_pSeconds;
	}
	long pNanoSeconds() {
		return m_pNanoSeconds;
	}

	PacketType packetType() {
		return m_packetType;
	}
	
	IPPacketType ipPacketType() {
		return m_ipPacketType;
	}
	
	u_short ipLength() {
		return m_ipLength;
	}
	
	string sourceMAC() {
		return m_sourceMAC;
	}
	
	string destinationMAC() {
		return m_destinationMAC;
	}
	
	string sourceIP() {
		return m_sourceIP;
	}
	
	string destinationIP() {
		return m_destinationIP;
	}
	
	u_int16_t sourcePort() {
		return m_sourcePort;
	}
	
	u_int16_t destinationPort() {
		return m_destinationPort;
	}
	
	bool fin() {
		return m_fin;
	}
	
	bool syn() {
		return m_syn;
	}
	
	bool rst() {
		return m_rst;
	}
	
	bool psh() {
		return m_psh;
	}
	
	bool ack() {
		return m_ack;
	}
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

	time_t m_pSeconds;
	long m_pNanoSeconds;
	
	bool m_fin;
	bool m_syn;
	bool m_rst;
	bool m_psh;
	bool m_ack;
	
	bool m_validPacket;
	
	bool _decodePacket(const pcap_pkthdr* header, const u_char* packetData) {
		// ensure that the packet header is freed
		//auto_ptr<const pcap_pkthdr> headerAP(header);
		// ensure that the packet data is freed
		//auto_ptr<const u_char> packetDataAP(packetData);
		
		// sanity check
		if(header->caplen < SIZE_ETHERNET) {
			cerr << "(PacketHeader::_decodePacket)-caplen (" << header->caplen << ") < ethernet header size (" << SIZE_ETHERNET << ")" << endl;
			return false;
		}
		
		struct ether_header *eptr = (struct ether_header *) packetData;
		
		// source/dest MAC
		// FIXME the calls below are *NOT* re-entrant!
		//m_sourceMAC		= ether_ntoa(eptr->ether_shost);
		//m_destinationMAC	= ether_ntoa(eptr->ether_dhost);
	
		if (ntohs (eptr->ether_type) == ETHERTYPE_IP) {
			// ip header
			const struct ip *ipHeader;
		
			// sanity check
			if(header->caplen < SIZE_ETH_IP_HEADER) {
				cerr << "(PacketHeader::_decodePacket)-caplen (" << header->caplen << ") < ethernet header size + ip header size (" << (SIZE_ETHERNET + sizeof(ip)) << ")" << endl;
				return false;
			}

			ipHeader = (struct ip*)(packetData + SIZE_ETHERNET);
			
			// sanity check
			// FIXME remove hard-coded values
			u_int16_t ipHeaderSize = ipHeader->ip_hl * 4;
			if (ipHeaderSize < 20) {
				cerr << "*** Invalid IP header length: " << ipHeaderSize << endl;;
				return false;
			}
	
			// packet type
			m_packetType = PACKET_IP;
			
			// source ip
			char srcAddress[17];
			strncpy(srcAddress, inet_ntoa(ipHeader->ip_src), 17);
			srcAddress[16] = 0;
			m_sourceIP = srcAddress;
			
			// destination ip
			char destAddress[17];
			strncpy(destAddress, inet_ntoa(ipHeader->ip_dst), 17);
			destAddress[16] = 0;
			m_destinationIP = destAddress;
	
			// length of the ip packet
			m_ipLength = ntohs(ipHeader->ip_len);
			//u_short ipId = ntohs(ipHeader->ip_id);
	
			if(ipHeader->ip_p == IPPROTO_TCP) {
				// sanity check
				if(header->caplen < SIZE_ETH_IP_TCP_HEADER) {
					cerr << "(PacketHeader::_decodePacket)-caplen (" << header->caplen << ") < ethernet header size + ip header size + tcp header size (" << SIZE_ETH_IP_TCP_HEADER << ")" << endl;
					return false;
				}
				
				const struct tcphdr* tcpHeader = (struct tcphdr*)(packetData + SIZE_ETHERNET + ipHeaderSize);

				// transport type
				m_ipPacketType = IP_TCP;
				
				m_sourcePort		= ntohs(tcpHeader->source);
				m_destinationPort	= ntohs(tcpHeader->dest);
	
				//u_int16_t seq		= ntohs(tcpHeader->seq);
				//u_int16_t ack_seq	= ntohs(tcpHeader->ack_seq);
				m_fin		= (ntohs(tcpHeader->fin) == 0) ? false : true;
				m_syn		= (ntohs(tcpHeader->syn) == 0) ? false : true;
				m_rst		= (ntohs(tcpHeader->rst) == 0) ? false : true;
				m_psh		= (ntohs(tcpHeader->psh) == 0) ? false : true;
				m_ack		= (ntohs(tcpHeader->ack) == 0) ? false : true;
				//u_int16_t urg		= ntohs(tcpHeader->urg);
				//u_int16_t res2		= ntohs(tcpHeader->res2);
	
			} else if(ipHeader->ip_p == IPPROTO_UDP) {
				// transport type
				m_ipPacketType = IP_UDP;
			} else if(ipHeader->ip_p == IPPROTO_ICMP) {
				// transport type
				m_ipPacketType = IP_ICMP;
			} else {
				// transport type
				m_ipPacketType = IP_UNKNOWN;
			}
			
		} else  if (ntohs (eptr->ether_type) == ETHERTYPE_ARP) {
			// packet type
			m_packetType = PACKET_ARP;
		} else {
			// packet type
			m_packetType = PACKET_UNKNOWN;
		}
		
		// flag a decoded packet
		m_validPacket = true;
		
		return m_validPacket;
	}
};

struct PacketHandlerData {
	ObjectQueue<PacketHeader*> packets;
};

#endif


/*
bool PacketDecodingThread::_decodePacket(PacketHeader* curPacket) {
	//cerr << "decoding packet" << endl;
	
	// this makes sure the curPacket is deleted when we're done
	auto_ptr<PacketHeader> curPacketAuto(curPacket);
	
	//fprintf(stderr, "%lu-pkt len-%.4d-", time(NULL), curPacket->packetHeader().len);
	//    printf("Recieved at ..... %s\n",ctime((const time_t*)&hdr.ts.tv_sec)); 
	//printf("Ethernet address length is %d\n",ETHER_HDR_LEN);
	
	struct ether_header *eptr = (struct ether_header *) curPacket->packetData();

	const struct ip *ipHeader;
	const struct tcphdr* tcpHeader;
	const char *payload;

	if (ntohs (eptr->ether_type) == ETHERTYPE_IP) {
		u_char* packetData = curPacket->packetData();
		ipHeader = (struct ip*)(packetData + SIZE_ETHERNET);

		//cerr << "IP-";
		u_int16_t ipHeaderSize = ipHeader->ip_hl * 4;
		if (ipHeaderSize < 20) {
			printf("   * Invalid IP header length: %u bytes\n", ipHeaderSize);
			return false;
		}

		char srcAddress[17];
		strncpy(srcAddress, inet_ntoa(ipHeader->ip_src), 17);
		srcAddress[16] = 0;
		char destAddress[17];
		strncpy(destAddress, inet_ntoa(ipHeader->ip_dst), 17);
		destAddress[16] = 0;

		u_short ipLength = ntohs(ipHeader->ip_len);
		u_short ipId = ntohs(ipHeader->ip_id);

		m_totalStats["ipTotalBytes"] += ipLength;

		if(ipHeader->ip_p == IPPROTO_TCP) {
			tcpHeader = (struct tcphdr*)(packetData + SIZE_ETHERNET + ipHeaderSize);
			u_int16_t sourcePort	= ntohs(tcpHeader->source);
			u_int16_t destPort	= ntohs(tcpHeader->dest);

			//u_int16_t seq		= ntohs(tcpHeader->seq);
			//u_int16_t ack_seq	= ntohs(tcpHeader->ack_seq);
			u_int16_t fin		= ntohs(tcpHeader->fin);
			u_int16_t syn		= ntohs(tcpHeader->syn);
			u_int16_t rst		= ntohs(tcpHeader->rst);
			u_int16_t psh		= ntohs(tcpHeader->psh);
			u_int16_t ack		= ntohs(tcpHeader->ack);
			//u_int16_t urg		= ntohs(tcpHeader->urg);
			//u_int16_t res2		= ntohs(tcpHeader->res2);

			//cerr << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") fin (" << fin << ") syn (" << syn << ") rst (" << rst << ") psh (" << psh << ") ack (" << ack << ")" << endl;

			//cerr << "TCP-src-" << srcAddress << "-dst-" << destAddress << "-srcPort-" << sourcePort << "-destPort-" << destPort << "-ip_len-" << ipLength << "-ip_id-" << ipId << "-";

			m_totalPortStats[sourcePort] += ipLength;
			m_totalPortStats[destPort] += ipLength;
			
			m_totalStatsByIp[srcAddress] += ipLength;
			m_totalStatsByIp[destAddress] += ipLength;
			
			// connection tracking
			TCPConnection* curConnection;
			
			// packetSource:
			//		true	== source address on connection maps to packet source address
			//		false	== source address on connection maps to packet destination address
			bool packetSource = true;
			if(m_connections.find(IPTCPPort(srcAddress, sourcePort, destAddress, destPort)) != m_connections.end()) {
				curConnection = m_connections[IPTCPPort(srcAddress, sourcePort, destAddress, destPort)];
			} else if(m_connections.find(IPTCPPort(destAddress, destPort, srcAddress, sourcePort)) != m_connections.end()) {
				curConnection = m_connections[IPTCPPort(destAddress, destPort, srcAddress, sourcePort)];
				
				// opposite direction from the original packet
				packetSource = false;
			} else {
				stringstream ss;
				ss << "new connection pair for source ip (" << srcAddress << ") port (" << sourcePort << ") dest ip (" << destAddress << ") port (" << destPort + ")" << endl;
				cerr << ss.str();
				curConnection = new TCPConnection(srcAddress, sourcePort, destAddress, destPort);

				m_connections[IPTCPPort(srcAddress, sourcePort, destAddress, destPort)] = curConnection;
			}

			// connection state
			if(packetSource) {
				curConnection->bytesInIncrement(ipLength);
				if(syn != 0) {
					curConnection->stateSourceSyn(true);
				}
				if(ack != 0) {
					curConnection->stateSourceAck(true);
				}
				if(fin != 0) {
					curConnection->stateSourceFin(true);
				}
				if(rst != 0) {
					curConnection->stateSourceReset(true);
				}
			} else {
				curConnection->bytesOutIncrement(ipLength);
				if(syn != 0) {
					curConnection->stateDestSyn(true);
				}
				if(ack != 0) {
					curConnection->stateDestAck(true);
				}
				if(fin != 0) {
					curConnection->stateDestFin(true);
				}
				if(rst != 0) {
					curConnection->stateDestReset(true);
				}
			}
			
			// debug output
			//cerr << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") ";
			if(curConnection->stateSourceReset() || curConnection->stateDestReset()) {
				stringstream ss;
				ss << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") RESET" << endl;
				cerr << ss.str();
				curConnection->connectionState(STATE_RESET);
			} else if(curConnection->stateSourceFin() && curConnection->stateDestFin()) {
				stringstream ss;
				ss << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") CLOSED" << endl;
				cerr << ss.str();
				//cerr << "closed";
				curConnection->connectionState(STATE_CLOSED);
			} else if(curConnection->stateSourceSyn() && curConnection->stateDestSyn() && curConnection->stateDestAck() && curConnection->stateSourceAck()) {
				//DbgMsg("connected");
				curConnection->connectionState(STATE_CONNECTED);
			} else if(curConnection->stateSourceSyn() || curConnection->stateDestSyn() || curConnection->stateDestAck() || curConnection->stateSourceAck()) {
				stringstream ss;
				ss << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") CONNECTING" << endl;
				cerr << ss.str();
				//cerr << "connection in progress";
				curConnection->connectionState(STATE_CONNECTING);
			} else if(curConnection->stateSourceFin() || curConnection->stateDestFin()) {
				//cerr << "connection closing";
				stringstream ss;
				ss << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") CLOSING" << endl;
				cerr << ss.str();
				curConnection->connectionState(STATE_CLOSING);
			} else {
				stringstream ss;
				ss << "srcAddress (" << srcAddress << ") destAddress (" << destAddress << ") sourcePort (" << sourcePort << ") destPort (" << destPort << ") UNKNOWN" << endl;
				cerr << ss.str();
				//cerr << "unknown";
				curConnection->connectionState(STATE_UNKNOWN);
			}
			
			//cerr << endl;
		} else if(ipHeader->ip_p == IPPROTO_UDP) {
			stringstream ss;
			ss << "UDP-src-" << srcAddress << "-dst-" << destAddress << "-ip_len-" << ipLength << "-ip_id-" << ipId << "-" << endl;
			cerr << ss.str();
		} else if(ipHeader->ip_p == IPPROTO_ICMP) {
			stringstream ss;
			ss << "ICMP-src-" << srcAddress << "-dst-" << destAddress << "-ip_len-" << ipLength << "-ip_id-" << ipId << "-" << endl;
			cerr << ss.str();
		} else {
			stringstream ss;
			ss << "UNKNOWN-src-" << inet_ntoa(ipHeader->ip_src) << "-dst-" << inet_ntoa(ipHeader->ip_dst) << "-ip_len-" << ipLength << "-ip_id-" << ipId << "-" << endl;
			cerr << ss.str();
		}
		
	} else  if (ntohs (eptr->ether_type) == ETHERTYPE_ARP) {
		cerr << "ARP-" << endl;
	} else {
		cerr << "UNKNOWN" << endl;
	}
	
	return true;
}
*/

//cerr << endl;

/* THANK YOU RICHARD STEVENS!!! RIP*/
/*
u_char *ptr = eptr->ether_dhost;
int i = ETHER_ADDR_LEN;
fprintf(stderr, "Dest/Source MAC-");
do{
	//fprintf(stderr, "%s%.2x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
	fprintf(stderr, "%s%.2x",":",*ptr++);
} while(--i>0);
fprintf(stderr, "-");

ptr = eptr->ether_shost;
i = ETHER_ADDR_LEN;
do{
	fprintf(stderr, "%s%.2x",":",*ptr++);
} while(--i>0);
fprintf(stderr, "\n");
*/

