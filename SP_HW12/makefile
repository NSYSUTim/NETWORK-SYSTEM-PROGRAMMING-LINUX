all: shm_server.o lookup5.o lookup6.o main.o
	gcc -o shm_server shm_server.c lookup5.c
	gcc -o shm_client main.c lookup6.c
clean:
	rm -f *.o shm_client  shm_server
