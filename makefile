CC=g++
CC_OPTIONS=-I. -I/usr/include -I/usr/include/pqxx
LD_OPTIONS=-lpthread -lpcap -lrt

TARGET_OBJS=PacketDecodingThread.o LatencyMonitoringPacketDecodingThread.o connectionstate.o iptcpport.o main.o tcpconnection.o ConnectionMap.o
TARGET=packet_decoder

$(TARGET):	$(TARGET_OBJS)
	echo building target
	$(CC) $(LD_OPTIONS) -o $(TARGET) $(TARGET_OBJS)

%.o: %.cpp
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

%.o: %.cpp %.h
	@echo Compiling file [$<] to [$@]
	$(CC) $(CC_OPTIONS) -c $< -o $@ || rm $@

clean:
	rm *.o

