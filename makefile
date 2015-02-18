CC=g++
CC_OPTIONS=-Iinclude -I/usr/include -I/usr/include/pqxx -D_BSD_SOURCE
#LD_OPTIONS=-lpthread -lpcap -lrt
LD_OPTIONS=-lpthread -lpcap

TARGET_OBJS=obj/PacketDecodingThread.o obj/LatencyMonitoringPacketDecodingThread.o obj/connectionstate.o obj/iptcpport.o obj/main.o obj/tcpconnection.o obj/ConnectionMap.o obj/timespechelpers.o obj/packet_data.o obj/clock.o
TARGET=packet_decoder

$(TARGET):	obj $(TARGET_OBJS)
	echo building target
	$(CC) -o $(TARGET) $(TARGET_OBJS) $(LD_OPTIONS)

obj:	
	mkdir obj

obj/%.o: src/%.cpp
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

obj/%.o: obj src/%.cpp include/%.h
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

clean:
	rm obj/*.o
	rm -f packet_decoder 

