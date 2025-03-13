all: usock_server.o lookup5.o main.o lookup7.o
	gcc -o usock_server usock_server.c lookup5.c
	gcc -o usock_client main.c lookup7.c
clean:
	rm -f *.o usock_server usock_client TUNNEL
