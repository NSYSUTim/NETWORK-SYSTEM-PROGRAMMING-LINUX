#include "seqnum.h"

#define MSG_KEY 0x1234

void errExit(const char err[]){
    printf("%s",err);   exit(1);
}

int main() {
    int msgid;
    struct request req;
    struct response resp;
    int seqNum = 0;

    // 創建System V消息佇列
    if ( (msgid = msgget(MSG_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1){
        if (errno == EEXIST) {
            msgid = msgget(MSG_KEY, S_IRUSR | S_IWUSR);
        }else{
            errExit("msgget");
        }
    }
        

    for (;;) {
        // server先接收client端請求
        if (msgrcv(msgid, &req, sizeof(struct request) - sizeof(long), 1, 0) == -1) {
            errExit("msgrcv");
        }

        resp.mtype = req.pid;
        resp.seqNum = seqNum;
        seqNum += req.seqLen;

        // 發送回應resp到對應的客戶端
        if (msgsnd(msgid, &resp, sizeof(struct response) - sizeof(long), 0) == -1) {
            fprintf(stderr, "Error sending response to client\n");
        }
    }

    return 0;
}

