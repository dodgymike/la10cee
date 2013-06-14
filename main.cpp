#include <pcap.h>
#include <iostream>
#include <vector>

#include "object_queue.h"
#include "connectionstate.h"
#include "iptcpport.h"
#include "tcpconnection.h"
#include "packet_data.h"
#include "PacketDecodingThread.h"
#include "LatencyMonitoringPacketDecodingThread.h"
//#include "DBThread.h"

char errbuf[PCAP_ERRBUF_SIZE];

using namespace std;

void packet_handler(u_char* dummy, const pcap_pkthdr* packet_header, const u_char* packet_data);

bool threadJumpStart( Thread* myThread )
{
	return myThread->start();
};

bool dumpInterfaces() {
	pcap_if_t* available_interfaces;
	
	int rv = -1;
	if((rv = pcap_findalldevs(&available_interfaces, errbuf)) != 0) {
		cerr << "can't open interfaces because [" << errbuf << "] and rv [" << rv << "]" << endl;
		return false;
	}
	
	pcap_if_t* cur_interface = available_interfaces;
	pcap_if_t* next_interface;
	while((cur_interface != NULL)) {
		cerr << "name [" << cur_interface->name << "]";
		if(cur_interface->description != NULL) {
			cerr << " description [" << cur_interface->description << "]";
		}
		cerr << endl;
		
		next_interface = cur_interface->next;
		cur_interface = next_interface;
	}
	
	return true;
}

int main(int argc, char* argv[]) {
	if(!dumpInterfaces()) {
		return -1;
	}

	errbuf[0] = 0;
	char* cur_interface_name = "lo";
	char* decoder_thread_type = "standard";

	cout << "checking args" << endl;
	if(argc > 2) {
		decoder_thread_type = argv[2];
		cout << "\tdecoder_thread_type (" << decoder_thread_type << ")" << endl;
	} 
	if(argc > 1) {
		cur_interface_name = argv[1];
		cout << "interface_name (" << cur_interface_name << ")" << endl;
	}
	
	pcap_t* live_interface = pcap_open_live(cur_interface_name, MAX_PACKET_CAPTURE_SIZE, true, 100, errbuf);
	if(live_interface == NULL) {
		cerr << "error opening interface [" << cur_interface_name << "] - failed with error [" << errbuf << "]" << endl;
		return -1;
	}
	
	// safety check
	if(strlen(errbuf) != 0) {
		cerr << "interface [" << cur_interface_name << "] open, but error received [" << errbuf << "], continuing" << endl;
	}
	
	u_char* packetHandlerData = (u_char*)new PacketHandlerData;
	int numPackets = 0;
	int totalPackets = 0;

	StatMaps* statMaps = new StatMaps();
	
	pthread_t decodingThread_handle;

	pthread_attr_t packet_thread_attributes;
	pthread_attr_init(&packet_thread_attributes);
	pthread_attr_setdetachstate(&packet_thread_attributes, PTHREAD_CREATE_DETACHED);

	// FIXME
	PacketDecodingThread packetDecoderThread((PacketHandlerData*)packetHandlerData, statMaps);
	LatencyMonitoringPacketDecodingThread latencyMonitoringPacketDecoderThread((PacketHandlerData*)packetHandlerData, statMaps);

	void* decoder_thread = NULL;
	if(strcmp("latency", decoder_thread_type) == 0) {
		cout << "using latency thread" << endl;
		decoder_thread = &latencyMonitoringPacketDecoderThread;
	} else {
		cout << "using decoder thread" << endl;
		decoder_thread = &packetDecoderThread;
	}

	pthread_create(&decodingThread_handle, &packet_thread_attributes, (void *(*)(void *))threadJumpStart, decoder_thread);

	//packetDecoderThread.run();

	//long startTime = time(NULL);
	while((numPackets = pcap_dispatch(live_interface, 1, packet_handler, packetHandlerData)) >= 0) {
		totalPackets += numPackets;
		cerr << "total packets [" << totalPackets << "]" << endl;
		
		/*
		if((time(NULL) - startTime) > 800) {
			break;
		}
		*/
	}
	cerr << "got numPackets [" << numPackets << "]" << endl;

	return 0;
}

void packet_handler(u_char* packetHandlerData_ptr, const pcap_pkthdr* packet_header, const u_char* packet_data) {
	PacketHandlerData* packetHandlerData = (PacketHandlerData*)packetHandlerData_ptr;
	
	cerr << "<packet_handler>\tpacket_header caplen [" << packet_header->caplen << "] len [" << packet_header->len << "]" << endl;
	timespec cur_time;
	clock_gettime(CLOCK_REALTIME, &cur_time);
	
	cout << "<main>\ttv_sec (" << cur_time.tv_sec << ") tv_nsec (" << cur_time.tv_nsec << ")" << endl;

	PacketHeader* newPacket = new PacketHeader(packet_header, packet_data);
	packetHandlerData->packets.push(newPacket);
}


