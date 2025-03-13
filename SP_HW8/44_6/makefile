all: get_num.o fifo_seqnum_client.o error_functions.o fifo_seqnum_server.o fifo_seqnum_bad_client.o
	gcc -o fifo_seqnum_client fifo_seqnum_client.c get_num.c error_functions.c
	gcc -o fifo_seqnum_server fifo_seqnum_server.c get_num.c error_functions.c
	gcc -o fifo_seqnum_bad_client fifo_seqnum_bad_client.c get_num.c error_functions.c
clean:
	rm -f *.o get_num fifo_seqnum_client error_functions fifo_seqnum_server fifo_seqnum_bad_client
