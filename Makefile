
all: daq daq_single daq_stream

daq_poll: daq_poll.c 
	gcc -lm -lcomedi daq.c -o daq

daq_single: daq_single.c
	gcc -lm -lcomedi daq_single.c -o daq_single

daq_stream: daq_stream.c
	gcc -lm -lcomedi daq_stream.c -o daq_stream
