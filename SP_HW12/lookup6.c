#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int shmid, semid;
	long key = strtol(resource, (char **)NULL, 0);
	static Dictrec * shm;
	struct sembuf grab    = {0, -1, SEM_UNDO};   /* mutex other clients  */
	struct sembuf release = {0, 1, SEM_UNDO};    /* release mutex        */
	struct sembuf alert   = {1, 2, SEM_UNDO};    /* wake up server       */
	struct sembuf await   = {1, 0, 0};           /* wait for server      */
	static int first_time = 1;

	if (first_time) {        /* attach to shared memory & semaphores */
		first_time = 0;

		/* Connect to shared memory. */
		if ((shmid = shmget(key, sizeof(Dictrec), 0666)) == -1)
			DIE("shmget");

		/* Get shared memory virtual address. */
		if ((shm = (Dictrec *)shmat(shmid, NULL, 0)) == (Dictrec *)-1)
			DIE("shmat");

		/* Get semaphore. */
		if ((semid = semget(key, 2, 0666)) == -1)
			DIE("semget");
	}

	/* Reserve semaphore so other clients will wait. */
	if (semop(semid, &grab, 1) == -1)
		DIE("semop grab");

	strcpy(shm->word, sought->word);

	/* Alert server. Bump semaphore 1 up by 2. */
	if (semop(semid, &alert, 1) == -1)
		DIE("semop alert");

	/* Wait for server to finish. */
	if (semop(semid, &await, 1) == -1)
		DIE("semop await");

	/* Done using the server. Release to other clients. */
	if (semop(semid, &release, 1) == -1)
		DIE("semop release");

	if (strcmp(shm->text, "XXXX") != 0) {
		strcpy(sought->text, shm->text);
		return FOUND;
	}

	return NOTFOUND;
}

