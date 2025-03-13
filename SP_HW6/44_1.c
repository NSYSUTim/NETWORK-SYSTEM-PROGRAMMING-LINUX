#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
//#define BUFSIZ 4096
//BUFSIZ 是內建的大小
int main() {
    int pfd[2],pfd2[2];

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(1);
    }
    if (pipe(pfd2) == -1) {
        perror("pipe2");
        exit(1);
    }
    pid_t pid;
    switch (pid=fork())
    {
    case -1:
        perror("fork");
        exit(1);
        break;
    default:
        //par read from stdin and write to pipe
        if(close(pfd[0])==-1){
            printf("close pfd[0]");
            exit(1);
        }
        if(close(pfd2[1])==-1){ //close要在迴圈外，不然會重複close
            printf("close pfd2[1]");
            exit(1);
        }
        ssize_t readbytes;
        char buf[BUFSIZ];
    while(1){
        memset(buf,'\0',BUFSIZ);
        if( (readbytes=read(STDIN_FILENO,buf,BUFSIZ))==-1){
            printf("read from stdin\n");   exit(1);
        }
        if(readbytes==0)    break;
        if(write(pfd[1],buf,readbytes)==-1){
            printf("write to pipe1\n");   exit(1);
        }
    //}
        //par read from pipe2 and write to stdout
        ssize_t readbytes1;
        char output[BUFSIZ];
        if( (readbytes1=read(pfd2[0],output,BUFSIZ))==-1){
            printf("read from pfd2[0]\n");   exit(1);
        }
        if(write(STDOUT_FILENO,output,readbytes1)==-1){
            printf("write to stdout\n");   exit(1);
        }
    }
        if(close(pfd[1])==-1){
            printf("close pfd[1]");
            exit(1);
        }
        if(close(pfd2[0])==-1){
            printf("close pfd2[0]");
            exit(1);
        }
        break;
    case 0:
        //read from pipe1 and convert to uppercase
        if(close(pfd[1])==-1){
            printf("close pfd[1]");
            exit(1);
        }
        if(close(pfd2[0])==-1){ //close要在迴圈外，不然會重複close
            printf("close pfd2[0]");
            exit(1);
        }
        char upper[BUFSIZ];
        ssize_t readbytes2;
    while(1){
        memset(upper,'\0',BUFSIZ);
        if( (readbytes2=read(pfd[0],upper,BUFSIZ)) ==-1){
            printf("read from pfd[0]\n");   exit(1);
        }
        if(readbytes2==0)   break;
        //read完跳出迴圈再close
        // if(close(pfd[0])==-1){
        //     printf("close pfd[0]");
        //     exit(1);
        // }
        int i=0;
        while(upper[i]!='\0'){
            upper[i]=toupper(upper[i]);
            i++;
        }
        //printf("upper: %s\n",upper);
        //send to pipe2
        if(write(pfd2[1],upper,strlen(upper))==-1){
            printf("write to pipe2\n");   exit(1);
        }
        //因為還要繼續寫入pipe2
        // if(close(pfd2[1])==-1){
        //     printf("close pfd2[1]");
        //     exit(1);
        // }
    }
        if(close(pfd[0])==-1){
            printf("close pfd[0]");
            exit(1);
        }
        if(close(pfd2[1])==-1){
            printf("close pfd2[1]");
            exit(1);
        }
        break;
    }

    return 0;
}