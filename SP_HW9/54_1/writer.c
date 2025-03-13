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
	int shmfd;
	struct shmseg *shmp;
	sem_t *write_sem, *read_sem;

	// 創建共享記憶體對象
	shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
	if(shmfd == -1)
		errExit("shm_open");

	if(ftruncate(shmfd, sizeof(struct shmseg)) == -1)
		errExit("ftruncate");

	// 映射共享記憶體
	shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if(shmp == MAP_FAILED)
		errExit("mmap");

	// 創建和初始化信號量
	write_sem = sem_open(WRITE_SEM_NAME, O_CREAT, 0600, 1);
	if(write_sem == SEM_FAILED)
		errExit("sem_open write");

	read_sem = sem_open(READ_SEM_NAME, O_CREAT, 0600, 0);
	if(read_sem == SEM_FAILED)
		errExit("sem_open read");

	// 向共享記憶體寫入資料
	while(1){
		if(sem_wait(write_sem) == -1)
			errExit("sem_wait write");

		shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
		if(shmp->cnt == -1)
			errExit("read");

		if(sem_post(read_sem) == -1)
			errExit("sem_post read");

		if(shmp->cnt == 0) 
			break;
	}

	// 清理資源
	if(munmap(shmp, sizeof(struct shmseg)) == -1)
		errExit("munmap");

	if(close(shmfd) == -1)
		errExit("close");

	if(sem_close(write_sem) == -1 || sem_close(read_sem) == -1)
		errExit("sem_close");

	exit(EXIT_SUCCESS);
}
