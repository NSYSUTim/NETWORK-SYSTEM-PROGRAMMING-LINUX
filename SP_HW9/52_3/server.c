#include "seqnum.h"

#define MAX_MSG_SIZE 1024

void errExit(const char *err) {
    perror(err);
    exit(EXIT_FAILURE);
}

// server發送當前seqnum，並更新加上client傳來的seqnum
int main() {
    mqd_t serverQueue;
    char clientQueueName[128];
    struct mq_attr attr;
    struct request req;
    struct response resp;
    int seqNum = 0;

    // 設定訊息佇列屬性
    attr.mq_flags = 0;               // 阻塞模式
    attr.mq_maxmsg = 10;             // 最大訊息數
    attr.mq_msgsize = MAX_MSG_SIZE;  // 每則訊息的最大大小
    attr.mq_curmsgs = 0;             // 當前訊息數

    // 創建伺服器的 POSIX 訊息佇列
    if ((serverQueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr)) == (mqd_t)-1) {
        errExit("mq_open");
    }

    for (;;) {
        // 從伺服器佇列中接收請求
        if (mq_receive(serverQueue, (char *)&req, MAX_MSG_SIZE, NULL) == -1) {
            errExit("mq_receive");
        }

        // 客戶端訊息佇列名稱
        snprintf(clientQueueName, sizeof(clientQueueName), "/seqnum_cl.%ld", (long)req.pid);

        // 回應client
        resp.seqNum = seqNum;
        seqNum += req.seqLen;

        // 發送回應到客戶端的 POSIX 訊息佇列
        mqd_t clientQueue;
        if ((clientQueue = mq_open(clientQueueName, O_WRONLY)) == (mqd_t)-1) {
            fprintf(stderr, "Error opening client queue: %s\n", clientQueueName);
            continue;
        }

        if (mq_send(clientQueue, (char *)&resp, sizeof(resp), 0) == -1) {
            fprintf(stderr, "Error sending response to client\n");
        }

        mq_close(clientQueue);
    }

    mq_close(serverQueue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}