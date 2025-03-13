/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int stat;

    /* Create a new child process.
     * Fill in code.
	 */
    //呼叫一次fork會有父行程並創造一個子行程
    switch (pid=fork()) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
            
        if( ! is_background(myArgv)){
                if(waitpid(pid,&stat,0)==-1){ //pid_t waitpid(pid_t pid, int *wstatus, int options); 不使用options，設為0
                    perror("waitpid");
                    exit(1);
                }
            
            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
			 */

            //same as man page's example of waitpid
            if(WIFEXITED(stat)){
                printf("exited, status=%d\n", WEXITSTATUS(stat));
            }else if (WIFSIGNALED(stat)){
                printf("killed by signal %d\n", WTERMSIG(stat));
            }else if (WIFSTOPPED(stat)){
                printf("stopped by signal %d\n", WSTOPSIG(stat));
            }else if (WIFCONTINUED(stat)){
                printf("continued\n");
            }
        }
            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
            if(is_background(myArgv)){
                int i=0;
				while(strcmp(myArgv[i],"&") != 0)   i++;
				myArgv[i] = (char*) NULL;
			}

            execvp(myArgv[0],myArgv);   //執行成功就結束    //讓子行程執行輸入行的第一個字串指令，像是ls等，此execvp成功則父行程之stat等於0，如為執行1,A,NN等非有效命令，則stat等於2
            /* Handle error return from exec */
			exit(errno);
    }
}

