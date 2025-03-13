#include "seqnum.h"

#define MSG_KEY 0x1234

void errExit(const char err[]){
    printf("%s",err);   exit(1);
}

int main(int argc, char *argv[]) {
    int msgid;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0){
        printf("%s [seq-len]\n", argv[0]);  exit(1);
    }

    // 打開消息佇列
    if ( (msgid = msgget(MSG_KEY, S_IRUSR | S_IWUSR)) == -1) {
        errExit("msgget");
    }

    req.mtype = 1;
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;    //沒給seq-len就設1

    // 發送請求到server
    if (msgsnd(msgid, &req, sizeof(struct request) - sizeof(long), 0) == -1) {
        errExit("msgsnd");
    }

    // 接收server的回應
    if (msgrcv(msgid, &resp, sizeof(struct response) - sizeof(long), req.pid, 0) == -1) {
        errExit("msgrcv");
    }

    printf("%d\n", resp.seqNum);
    return 0;
}


