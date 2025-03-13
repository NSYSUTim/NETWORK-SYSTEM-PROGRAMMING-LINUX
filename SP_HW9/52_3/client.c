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
}