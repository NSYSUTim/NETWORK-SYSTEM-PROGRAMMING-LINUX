<<<<<<< HEAD
/* 
 * client program of 52_3
 */
#include <unistd.h>
#include <mqueue.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MSG_SIZE 2048
#define SERVER_MQ_NAME "/server_mq"
#define CLIENT_MQ_NAME "/client_mq"


struct request_msg{
	pid_t pid;
	char pathname[PATH_MAX];
};

struct response_msg{
	char data[MSG_SIZE];
};
int main(int argc, char *argv[]){
	mqd_t mq_server, mq_client;
	char mq_client_name[1024] = {'\0'};


	pid_t client_pid = getpid();
	int req_file_fd, read_size, receive_byte_num = 0, receive_msg_num = 0;
	struct request_msg client_req;
	struct response_msg client_res;
	struct mq_attr attr;
	
	if(argc != 2){
		perror("invalid argument number");
		exit(1);
	}

	client_req.pid = client_pid;
	strcpy(client_req.pathname, argv[1]);

	snprintf(mq_client_name, 1024, "%s%d", CLIENT_MQ_NAME, client_pid);
	//fprintf(stderr, "client_name: %s\n", mq_client_name);


	memset(&attr, 0, sizeof(struct mq_attr));
	attr.mq_msgsize = sizeof(struct response_msg);
	attr.mq_maxmsg = 4;

	if((mq_client = mq_open(mq_client_name, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr)) == (mqd_t)-1){
		perror("mq_open");
		exit(1);
	}

	if((mq_server = mq_open(SERVER_MQ_NAME, O_WRONLY)) == (mqd_t)-1){
		perror("mq_open");
		exit(1);
	}

	if(mq_send(mq_server, (char *)&client_req, sizeof(struct request_msg), 0) == -1){
		perror("mq_send");
		exit(1);
	}

	while(1){
		if((read_size = mq_receive(mq_client, (char *)&client_res, sizeof(struct response_msg), 0)) == -1){
			perror("mq_receive");
			exit(1);
		}
		if(client_res.data[0] == '\0' ){
			break;
		}
		receive_byte_num += strlen(client_res.data);
		// receive_byte_num += read_size;
		receive_msg_num += 1;
		// fprintf(stderr, "message get\n");
	}
	fprintf(stderr, "Reiceived %d bytes (%d messages)\n", receive_byte_num, receive_msg_num);

	if(mq_close(mq_server) == -1){
		perror("mq_close");
		exit(1);
	}
	if(mq_close(mq_client) == -1){
		perror("mq_client");
		exit(1);
	}
	if(mq_unlink(mq_client_name) == -1){
		perror("mq_client_name");
		exit(1);
	}

	exit(EXIT_SUCCESS);

=======
#include "seqnum.h"

#define MAX_MSG_SIZE 1024

void errExit(const char *err) {
    perror(err);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    mqd_t serverQueue, clientQueue;
    char clientQueueName[128];
    struct mq_attr attr;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("%s [seq-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 設定訊息佇列屬性
    attr.mq_flags = 0;               // 阻塞模式
    attr.mq_maxmsg = 10;             // 最大訊息數
    attr.mq_msgsize = MAX_MSG_SIZE;  // 每則訊息的最大大小
    attr.mq_curmsgs = 0;             // 當前訊息數

    // 創建客戶端的 POSIX 訊息佇列
    snprintf(clientQueueName, sizeof(clientQueueName), "/seqnum_cl.%ld", (long)getpid());
    if ((clientQueue = mq_open(clientQueueName, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr)) == (mqd_t)-1) {
        errExit("mq_open client");
    }

    // 打開伺服器的 POSIX 訊息佇列
    if ((serverQueue = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == (mqd_t)-1) {
        errExit("mq_open server");
    }

    // 發送請求到server
    req.pid = getpid();
    // req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;  // 沒給 seq-len 就設 1

    int fd;
    if( (fd=open(argv[1],O_RDONLY,0)) ==-1){
        errExit("server open");
    }
    int filsiz;
    filsiz = lseek(fd,0,SEEK_END);
    req.seqLen = filsiz;

    if (mq_send(serverQueue, (char *)&req, sizeof(req), 0) == -1) {
        errExit("mq_send");
    }

    // 接收伺服器的回應
    if (mq_receive(clientQueue, (char *)&resp, MAX_MSG_SIZE, NULL) == -1) {
        errExit("mq_receive");
    }

    printf("Received seq_num: %d\n", resp.seqNum);

    mq_close(clientQueue);
    mq_unlink(clientQueueName);
    return 0;
>>>>>>> 65e707a (提交專案)
}