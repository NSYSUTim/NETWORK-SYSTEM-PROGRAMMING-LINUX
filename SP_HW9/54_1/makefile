all: reader.o writer.o
	gcc -o reader reader.c
	gcc -o writer writer.c
	rm -f /dev/shm/sem.sem_key_read
	rm -f /dev/shm/sem.sem_key_write
	rm -f /dev/shm/sem.sem_read
	rm -f /dev/shm/sem.sem_write
	rm -f /dev/shm/shm_name
clean:
	rm -f *.o reader writer
