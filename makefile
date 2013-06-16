CC=g++
CC_OPTIONS=-Iinclude -I/usr/include -I/usr/include/pqxx
LD_OPTIONS=-lpthread -lpcap -lrt

TARGET_OBJS=obj/PacketDecodingThread.o obj/LatencyMonitoringPacketDecodingThread.o obj/connectionstate.o obj/iptcpport.o obj/main.o obj/tcpconnection.o obj/ConnectionMap.o
TARGET=packet_decoder

$(TARGET):	obj $(TARGET_OBJS)
	echo building target
	$(CC) $(LD_OPTIONS) -o $(TARGET) $(TARGET_OBJS)

obj:	
	mkdir obj

obj/%.o: src/%.cpp
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

obj/%.o: obj src/%.cpp include/%.h
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

clean:
	rm packet_decoder
	rm obj/*.o

