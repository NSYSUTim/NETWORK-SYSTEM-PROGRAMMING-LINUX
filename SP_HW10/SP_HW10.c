#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define SHM_NAME "shm_hw"
#define SHM_NAME_CNT "shm_hw_cnt"
#define BUFSIZE 80 // 單筆消息大小

struct shmseg {
	int msg_num;         // 消息編號
	char buf[BUFSIZE];   // 消息內容
};

struct shmseg_cnt {
	int cnt;        // 接收的消息數量
	int loss_cnt;   // 消息丟失的數量
};

struct shmseg *shmp;        // 消息內容的共享記憶體
struct shmseg_cnt *shmp_cnt; // 記錄消息數量的共享記憶體
int b;

// 信號處理函數
void handler(int signo, siginfo_t *info, void *ctx){    //sigaction的sa_sigaction有union sigval si_value(變數);可記錄值
	int message_index = info->si_value.sival_int % b;   //sigqueue裡有說明union sigval變數有成員sival_int可用
	if(info->si_value.sival_int == shmp[message_index].msg_num){
		shmp_cnt->cnt++;	//修改 shared memory 內容方法
		//printf("received: %s\n", shmp[message_index].buf); //顯示接收的消息
	}else{
		shmp_cnt->loss_cnt++;
        printf("Lost message: %d\n", info->si_value.sival_int);  // 顯示丟失的消息
	}
}

int main(int argc, char *argv[]){
	if(argc != 5){
		printf("Usage: %s <資料數量 M> <傳送間隔 R> <consumer數量 N> <buffer size B>\n", argv[0]);
		exit(1);
	}

	long m = atol(argv[1]);
	long r = atol(argv[2]);
	long n = atol(argv[3]);
	b = atol(argv[4]);

	// 設置共享記憶體
	int shmid, shmid_cnt;
	if((shmid=shm_open(SHM_NAME, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) ==-1){
		perror("shm_open");
		exit(1);
	}
	if(ftruncate(shmid, b * sizeof(struct shmseg)) == -1){
		perror("ftruncate");
		exit(1);
	}
	if((shmid_cnt=shm_open(SHM_NAME_CNT, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) ==-1){
		perror("shm_open_cnt");
		exit(1);
	}
	if(ftruncate(shmid_cnt, sizeof(struct shmseg_cnt)) ==-1){
		perror("ftruncate_cnt");
		exit(1);
	}

	// 映射共享記憶體
	if((shmp=mmap(NULL, b * sizeof(struct shmseg), PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	if((shmp_cnt = mmap(NULL, sizeof(struct shmseg_cnt), PROT_READ | PROT_WRITE, MAP_SHARED, shmid_cnt, 0)) == MAP_FAILED){
		perror("mmap_cnt");
		exit(1);
	}

	memset(shmp_cnt, 0, sizeof(struct shmseg_cnt)); // 初始化計數記憶體

	// 配置信號處理
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = handler;  //要設置sa_flags才可以用sa_sigaction
	sa.sa_flags = SA_SIGINFO;
	if(sigaction(SIGUSR1, &sa, NULL) ==-1){ //不需要oldact，寫NULL
		perror("sigaction");
		exit(1);
	}

    printf("Please wait\n");
	// 創建消費者進程
	pid_t pid_array[n];
	int i;
    for(i=0;i<n;i++){
		pid_t pid=fork();
		if(pid==-1){
			perror("fork");
			exit(1);
		}else if(pid == 0){ // 消費者進程
			while(1)    pause(); // consumer等待(被動觸發)，直到producer發送信號給這個consumer
			exit(0);    // 成功退出
		}else{
			pid_array[i] = pid; //紀錄消費者的PID，等一下才可以發送信號
		}
	}

	// 生產者發送 m 則消息
	for(i=0;i<m;i++){
		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);

        //共享記憶體內容
		snprintf(shmp[i % b].buf, BUFSIZE, "This is message %d", i);    // i % b，共享記憶體不能超過b大小
		shmp[i % b].msg_num = i;    

		union sigval mysigval;  //信號帶的值
		mysigval.sival_int = i;
		//生產者發送這則訊息給 n 個consumer
        int j;
        for(j=0;j<n;j++){
            //printf("Consumer %d ",j);
			if(sigqueue(pid_array[j], SIGUSR1, mysigval) ==-1){ // 用 sigqueue 傳送信號，不用kill，因為信號要帶值(當前訊息編號)
				perror("sigqueue");
			}
		}

		clock_gettime(CLOCK_REALTIME, &end);
		long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		if(elapsed_us < r * 1000)  usleep(r * 1000 - elapsed_us); // 保持傳送間隔 R
	}

	// 計算結果
	int total_messages = m * n;
	double loss_rate = 1.0 - ((double)shmp_cnt->cnt / total_messages);
    fprintf(stderr, "M: %ld R: %ldms N: %ld B: %d\n-------------------------------\n\n", m, r, n, b);
	fprintf(stderr, "Total messages: %d\n", total_messages);
	fprintf(stderr, "Received messages: %d\n", shmp_cnt->cnt);
	fprintf(stderr, "Loss rate: %.6f\n", loss_rate);
    printf("-------------------------------\n");
    printf("Lost messages: %d\n",shmp_cnt->loss_cnt);

	// 清理資源
	if(munmap(shmp, b * sizeof(struct shmseg)) == -1)   perror("munmap");
	if(munmap(shmp_cnt, sizeof(struct shmseg_cnt)) == -1)   perror("munmap_cnt");
	if(shm_unlink(SHM_NAME) == -1)  perror("shm_unlink");
	if(shm_unlink(SHM_NAME_CNT) == -1)  perror("shm_unlink_cnt");
	for(i=0;i<n;i++)  kill(pid_array[i], SIGKILL); // 終止消費者進程
	
    return 0;
}
