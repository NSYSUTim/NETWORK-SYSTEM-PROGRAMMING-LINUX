/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"




/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_echo(char **argv) {
  	/* Fill in code. */
	int n,k=1;				//n代表要找的位置，k代表要查找或印出的命令列起始位置
	if(strcmp(argv[1],"-n")==0){
		n= atoi(argv[2]) -0 + 2 ;
		k+=2;
	}
	//printf("%ld\n%s\n",sizeof(argv),argv[sizeof(argv)]);
	//sizeof不是二維陣列長度
	if(k==1){//print all
		int i;
		for(i=k;argv[i]!=NULL;i++){
			if(i!=k)	printf(" ");
			printf("%s",argv[i]);
		}printf("\n");
	}else{
		int i,len=0;
		for(i=0;argv[i]!=NULL;i++){
			len++;
		}
		if(n<=2){
			perror("-n num, num is too small\n");
			exit(1);
		}else if(n>=len){
			perror("-n num, num is too big\n");
			exit(1);
		}
		printf("%s\n",argv[n]);		//print 指定位置之字串
	}
	return;
}
/* Fill in code. */
const void bi_exit(char **argv){
	exit(1);
}

/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {		//cmd 的 { keyword,(*do_it) }

	/* Fill in code. */
	{"exit",bi_exit},			//不能用exit(1)，不然會直接執行
	{"quit",bi_exit},
	{"logout",bi_exit},
	{"bye",bi_exit},
	{ "echo", bi_echo },		/* When "echo" is typed, bi_echo() executes.  */
	{ NULL, NULL }				/* NULL terminated. */			//代表達到cmd結尾
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
}

