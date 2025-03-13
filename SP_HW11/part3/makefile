all: udp_server.o lookup5.o main.o lookup9.o
	gcc -o udp_server udp_server.c lookup5.c
	gcc -o udp_client main.c lookup9.c
clean:
	rm -f *.o udp_server udp_client
