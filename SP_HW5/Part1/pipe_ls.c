#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){

    pid_t pid;
    int pfd[2];

    if (pipe(pfd) == -1) {  //pipefd[0]先是讀再寫pipefd[1]
        perror("pipe");
        exit(1);
    }

    //呼叫一次fork會有父行程並創造一個子行程
    switch (pid=fork()) {
        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            break;
        // child
        case 0 :
            if(close(pfd[0])==-1){  //pfd[0]是讀端
                printf("close 1");
                exit(1);
            }
            dup2(pfd[1],STDOUT_FILENO);
            if(close(pfd[1])==-1){
                printf("close 2");
                exit(1);
            }
            execlp("ls", "ls", "-l", (char *)NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
    }
    if(close(pfd[1])==-1){  //pfd[1]是寫端
        printf("close 2");
        exit(1);
    }

    char buf[1024];
    read(pfd[0],buf,1024);
    puts(buf);
    if(close(pfd[0])==-1){  //pfd[0]是讀端
        printf("close 1");
        exit(1);
    }

    if (waitpid(pid, (int *)NULL, 0) == -1) {
        perror("waitpid");
        exit(1);
    }

    return 0;
}