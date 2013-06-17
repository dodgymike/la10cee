la10cee
=======

Latency and connection rate stats generator, based on libpcap


Run it like so:
sudo ./packet_decoder <interface> latency <filter string>
where interface could be something like 'eth0' and filter string could be 'port 80' (quotes required!).

It logs stats to syslog eg:
Jun 17 20:55:33 localhost latency-monitor[10183]: type=timing,srcip=192.168.12.117,srcport=56668,destip=196.23.168.155,destport=80,synack=0:34238826,ackack=0:43917
'synack' is the time difference between syn and syn/ack, ackack is the diff between syn/ack and ack in <seconds>:<nanoseconds>.

Building it should be easy if you've got libpcap dev headers installed. Otherwise it's self-contained.

If you make changes, please fork the project on github, make changes and submit a pull request to me. I'll be adding little features (tcp window size/ack latency/etc) as I find the time.
