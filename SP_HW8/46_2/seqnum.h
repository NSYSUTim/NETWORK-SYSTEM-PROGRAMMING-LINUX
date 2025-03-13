#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define CLIENT_FIFO_TEMPLATE "seqnum_cl.%ld"  // 用於創建客戶端 FIFO 的模板

// 請求訊息結構
struct request {
    long mtype;  // 訊息類型（1）
    pid_t pid;   // 客戶端 PID
    int seqLen;  // 請求的序列長度
};

// 回應訊息結構
struct response {
    long mtype;  // 訊息類型（2）
    int seqNum;  // server分配的序號
};

