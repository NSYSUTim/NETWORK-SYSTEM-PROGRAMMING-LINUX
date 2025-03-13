all: isock_server.o lookup5.o main.o lookup8.o
	gcc -o isock_server isock_server.c lookup5.c
	gcc -o isock_client main.c lookup8.c
clean:
	rm -f *.o isock_server isock_client 

