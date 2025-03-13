#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define SERVER_QUEUE_NAME "/seqnum_server_queue"  // 伺服器的消息佇列名稱

// 請求訊息結構
struct request {
    pid_t pid;   // 客戶端 PID
    int seqLen;  // 請求的序列長度
};

// 回應訊息結構
struct response {
    int seqNum;  // server 給的序號
};