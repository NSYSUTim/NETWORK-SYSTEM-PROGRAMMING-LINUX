#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#define TESTSIG SIGUSR1

static void handler(int sig){
    printf("Hello, I'm signal handler. MF!\n");
}
void errExit(const char s[]){
    printf("%s\n",s); exit(1);
}

int main(int argc, char *argv[]){

    if ( argc != 2 ){
        printf("Usage: <file> exec_nums\n");    exit(1);
    }

    int exec_nums = atoi(argv[1]);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(TESTSIG, &sa, NULL) == -1)
        errExit("sigaction");

    sigset_t blockedMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1)
        errExit("sigprocmask");

    struct timeval start, end;
    gettimeofday(&start, NULL);
    siginfo_t info;

    pid_t childPid ;
    switch (childPid = fork()) {
        case -1: errExit("fork");

        case 0:     /* child */
            for (int scnt = 0; scnt < exec_nums; scnt++) {
                if (kill(getppid(), TESTSIG) == -1)     // kill 是 發送信號 到該行程
                    errExit("kill");
                if (sigwaitinfo(&blockedMask,&info) == -1 ){
                    errExit("sigwaitinfo");     printf("errno:%d",errno);
                }
            }
            gettimeofday(&end, NULL);
            printf("The time elapsed after sending signals %d times between parent and child : %ld(sec) or %ld(microsec)\n",exec_nums,end.tv_sec-start.tv_sec,end.tv_usec-start.tv_usec);
            exit(1);

        default: /* parent */
            for (int scnt = 0; scnt < exec_nums; scnt++) {
                if (sigwaitinfo(&blockedMask,&info) == -1){
                    errExit("sigwaitinfo");     printf("errno:%d",errno);
                }
                if (kill(childPid, TESTSIG) == -1)
                    errExit("kill");
            }
            exit(1);
    }
    
}
