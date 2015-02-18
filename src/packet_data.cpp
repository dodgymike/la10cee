#include "packet_data.h"

PacketHeader::PacketHeader(const pcap_pkthdr* header, const u_char* data, timespec receivedTime) {
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

	m_receivedTime = receivedTime;
	
	_decodePacket(header, data);
}

PacketHeader::~PacketHeader() {
}
	
timespec PacketHeader::receivedTime() {
	return m_receivedTime;
}

PacketType PacketHeader::packetType() {
	return m_packetType;
}
	
IPPacketType PacketHeader::ipPacketType() {
	return m_ipPacketType;
}
	
u_short PacketHeader::ipLength() {
	return m_ipLength;
}
	
string PacketHeader::sourceMAC() {
	return m_sourceMAC;
}
	
string PacketHeader::destinationMAC() {
	return m_destinationMAC;
}
	
string PacketHeader::sourceIP() {
	return m_sourceIP;
}
	
string PacketHeader::destinationIP() {
	return m_destinationIP;
}
	
u_int16_t PacketHeader::sourcePort() {
	return m_sourcePort;
}
	
u_int16_t PacketHeader::destinationPort() {
	return m_destinationPort;
}
	
bool PacketHeader::fin() {
	return m_fin;
}
	
bool PacketHeader::syn() {
	return m_syn;
}
	
bool PacketHeader::rst() {
	return m_rst;
}
	
bool PacketHeader::psh() {
	return m_psh;
}
	
bool PacketHeader::ack() {
	return m_ack;
}
	
bool PacketHeader::_decodePacket(const pcap_pkthdr* header, const u_char* packetData) {
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
			
			m_sourcePort		= ntohs(tcpHeader->th_sport);
			m_destinationPort	= ntohs(tcpHeader->th_dport);

			//u_int16_t seq		= ntohs(tcpHeader->seq);
			//u_int16_t ack_seq	= ntohs(tcpHeader->ack_seq);
/*
			m_fin		= (ntohs(tcpHeader->fin) == 0) ? false : true;
			m_syn		= (ntohs(tcpHeader->syn) == 0) ? false : true;
			m_rst		= (ntohs(tcpHeader->rst) == 0) ? false : true;
			m_psh		= (ntohs(tcpHeader->psh) == 0) ? false : true;
			m_ack		= (ntohs(tcpHeader->ack) == 0) ? false : true;
			//u_int16_t urg		= ntohs(tcpHeader->urg);
			//u_int16_t res2		= ntohs(tcpHeader->res2);
*/

			m_fin = ((tcpHeader->th_flags & TH_FIN) == 0) ? false : true;
			m_syn = ((tcpHeader->th_flags & TH_SYN) == 0) ? false : true;
			m_rst = ((tcpHeader->th_flags & TH_RST) == 0) ? false : true;
			m_psh = ((tcpHeader->th_flags & TH_PUSH) == 0) ? false : true;
			m_ack = ((tcpHeader->th_flags & TH_ACK) == 0) ? false : true;

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

