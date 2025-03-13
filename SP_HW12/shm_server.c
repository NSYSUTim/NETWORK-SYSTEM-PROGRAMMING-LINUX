#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

int main(int argc, char **argv) {
	int shmid, semid;
	long key;
	int fd;
	Dictrec *shm;
	struct stat stbuff;
	unsigned short vals[2] = {1, 0};        /* Initial values of semaphores */

	// semctl 要用的
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	} setit;

	struct sembuf wait = {1, -1, SEM_UNDO}; /* used BOTH to wait & notify */

	if (argc != 3) {
		fprintf(stderr, "Usage : %s <dictionary source> <Resource/Key>\n", argv[0]);
		exit(errno);
	}

	/* Verify database resource is present. */
	if (stat(argv[1], &stbuff) == -1)
		DIE(argv[1]);

	/* Get key from commandline argument. */
	key = strtol(argv[2], (char **)NULL, 0);

	/* Map one record's worth of shared memory. */
	if ((shmid = shmget(key, sizeof(Dictrec), IPC_CREAT | 0666)) == -1)	//shared memory
		DIE("shmget");

	/* Get shared memory virtual address. */
	if ((shm = (Dictrec *)shmat(shmid, NULL, 0)) == (Dictrec *)-1)
		DIE("shmat");

	/* Allocate a group of two semaphores. */
	if ((semid = semget(key, 2, IPC_CREAT | 0666)) == -1)	//semaphore
		DIE("semget");

	/* Set up semaphore group. */
	setit.array = vals;
	if (semctl(semid, 0, SETALL, setit) == -1)
		DIE("semctl");

	/* Main working loop */
	for (;;) {
		/* Wait for client. */
		if (semop(semid, &wait, 1) == -1)
			DIE("semop wait");

		/* Do the lookup. */
		switch (lookup(shm, argv[1])) {
			case FOUND:
				break;
			case NOTFOUND:
				strcpy(shm->text, "XXXX");
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}

		/* Notify client. */
		if (semop(semid, &wait, 1) == -1)
			DIE("semop notify");
	}
}

