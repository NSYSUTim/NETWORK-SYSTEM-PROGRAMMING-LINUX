#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

#define BUF_SIZE 4096
#define SHM_NAME "/shm_name"
#define WRITE_SEM_NAME "/sem_write"
#define READ_SEM_NAME "/sem_read"

struct shmseg{
	int cnt;				
	char buf[BUF_SIZE];	
};

void errExit(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	int shmfd, bytes, xfrs;
	struct shmseg *shmp;
	sem_t *write_sem, *read_sem;

	// 打開共享記憶體對象
	shmfd = shm_open(SHM_NAME, O_RDONLY, 0);	
	if(shmfd == -1)
		errExit("shm_open");

	// 映射共享記憶體
	shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shmfd, 0);
	if(shmp == MAP_FAILED)
		errExit("mmap");

	// 打開信號量
	write_sem = sem_open(WRITE_SEM_NAME, 0);
	if(write_sem == SEM_FAILED)
		errExit("sem_open write");

	read_sem = sem_open(READ_SEM_NAME, 0);
	if(read_sem == SEM_FAILED)
		errExit("sem_open read");

	// 從共享記憶體讀取資料
	for(xfrs = 0, bytes = 0; ; xfrs++){
		if(sem_wait(read_sem) == -1)
			errExit("sem_wait read");

		if(shmp->cnt == 0) 
			break;

		bytes+=shmp->cnt;

		if(write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
			errExit("write");

		if(sem_post(write_sem) == -1)
			errExit("sem_post write");
	}

	fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);

	// 清理資源
	if(munmap(shmp, sizeof(struct shmseg)) == -1)
		errExit("munmap");

	if(close(shmfd) == -1)
		errExit("close");

	if(sem_close(write_sem) == -1 || sem_close(read_sem) == -1)
		errExit("sem_close");

	exit(EXIT_SUCCESS);
}
