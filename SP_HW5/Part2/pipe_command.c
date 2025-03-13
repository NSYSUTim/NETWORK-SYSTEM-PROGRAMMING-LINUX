/* 
 * pipe_command.c  :  deal with pipes
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0

void pipe_and_exec(char **myArgv) {
  	int pipe_argv_index = pipe_present(myArgv);
  	int pipefds[2];
	char **left_argv;
	char **right_argv;

  	switch (pipe_argv_index) {

    	case -1:	/* Pipe at beginning or at end of argv;  See pipe_present(). */
      		fputs ("Missing command next to pipe in commandline.\n", stderr);
      		errno = EINVAL;	/* Note this is NOT shell exit. */
      		break;
    	case 0:	/* No pipe found in argv array or at end of argv array.
			See pipe_present().  Exec with whole given argv array. */
			execvp(myArgv[0],myArgv);
      		break;
    	default:	/* Pipe in the middle of argv array.  See pipe_present(). */

      		/* Split arg vector into two where the pipe symbol was found.
       		 * Terminate first half of vector.
			 *
       		 * Fill in code. */
			if( (left_argv=malloc(pipe_argv_index*sizeof(char*)))==NULL){
				printf("malloc left_argv failed");
				exit(1);
			}
			int i;
			for(i=0;i<pipe_argv_index;i++){
				if( (left_argv[i]=malloc(strlen(myArgv[i])*sizeof(char)))==NULL ){
					printf("malloc left_argv[%d] failed",i);
					exit(1);
				}
				strcpy(left_argv[i],myArgv[i]);	//會有'\0'結尾，strncpy才有可能沒有
			}
			left_argv[i+1]=(char*)NULL;

			if( (right_argv=malloc(1*sizeof(char*)))==NULL){
				printf("malloc right_argv failed");
				exit(1);
			}
			i=0;
			while(myArgv[i+pipe_argv_index+1]!=NULL){
				if( (right_argv=realloc(right_argv,(i+1)*sizeof(char*)))==NULL){
					printf("realloc right_argv failed");
					exit(1);
				}
				if( (right_argv[i]=malloc(strlen(myArgv[i+pipe_argv_index+1])*sizeof(char)))==NULL ){
					printf("malloc left_argv[%d] failed",i);
					exit(1);
				}
				strcpy(right_argv[i],myArgv[i+pipe_argv_index+1]);
				i++;
			}
			right_argv[i+1]=(char*)NULL;

      		/* Create a pipe to bridge the left and right halves of the vector. 
			 *
			 * Fill in code. */
			if(pipe(pipefds) == -1) {
            	perror("pipe");
				exit(EXIT_FAILURE);
           	}
      		/* Create a new process for the right side of the pipe.
       		 * (The left side is the running "parent".)
       		 *
			 * Fill in code to replace the underline. */
			pid_t pid;
      		switch(pid=fork()) {

        		case -1 :
	  				break;
        		/* Talking parent.  Remember this is a child forked from shell. */
        		default :

	  				/* - Redirect output of "parent" through the pipe.
	  				 * - Don't need read side of pipe open.  Write side dup'ed to stdout.
	 	 			 * - Exec the left command.
					 *
					 * Fill in code. */
	  				if(close(pipefds[0])==-1){
						printf("close 1");
						exit(1);
					}
					dup2(pipefds[1],STD_OUTPUT);
					if(close(pipefds[1])==-1){
						printf("close 2");
						exit(1);
					}
					execvp(left_argv[0],left_argv);
					break;

        		/* Listening child. */
        		case 0 :

	  				/* - Redirect input of "child" through pipe.
					  * - Don't need write side of pipe. Read side dup'ed to stdin.
				  	 * - Exec command on right side of pipe and recursively deal with other pipes
					 *
					 * Fill in code. */
					if(close(pipefds[1])==-1){
						printf("close 2");
						exit(1);
					}
					dup2(pipefds[0],STD_INPUT);
					if(close(pipefds[0])==-1){
						printf("close 1");
						exit(1);
					}
					execvp(right_argv[0],right_argv);
          			pipe_and_exec(&myArgv[pipe_argv_index+1]);
			}
	}
	perror("Couldn't fork or exec child process");
  	exit(errno);
}
