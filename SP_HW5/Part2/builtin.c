/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#include "shell.h"

/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/
static void bi_builtin(char ** argv);	/* "builtin" command tells whether a command is builtin or not. */
static void bi_cd(char **argv) ;		/* "cd" command. */
static void bi_echo(char **argv);		/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_hostname(char ** argv);	/* "hostname" command. */
static void bi_id(char ** argv);		/* "id" command shows user and group of this process. */
static void bi_pwd(char ** argv);		/* "pwd" command. */
static void bi_quit(char **argv);		/* quit/exit/logout/bye command. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
  	char * keyword;					/* When this field is argv[0] ... */
  	void (* do_it)(char **);		/* ... this function is executed. */
} inbuilts[] = {
  	{ "builtin",    bi_builtin },   /* List of (argv[0], function) pairs. */

    /* Fill in code. */
    { "echo",       bi_echo },
    { "quit",       bi_quit },
    { "exit",       bi_quit },
    { "bye",        bi_quit },
    { "logout",     bi_quit },
    { "cd",         bi_cd },
    { "pwd",        bi_pwd },
    { "id",         bi_id },
    { "hostname",   bi_hostname },
    {  NULL,        NULL }          /* NULL terminated. */
};


static void bi_builtin(char ** argv) {
	/* Fill in code. */
	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,argv[1]) == 0) {
      		printf("%s is a builtin feature.\n",argv[1]);
			return;
	   	}
	printf("%s is NOT a builtin feature.\n",argv[1]);

	// int i;
	// for(i=0;i<sizeof(inbuilts)/sizeof(inbuilts[0]);i++){
	// 	if(strcmp(inbuilts[i].keyword,argv[1])==1){
	// 		printf("%s is a builtin feature.\n",argv[1]);
	// 		return;
	// 	}
	// }printf("%s is NOT a builtin feature.\n",argv[1]);
}

static void bi_cd(char **argv) {	//printf("%s\n",argv[1]);
	/* Fill in code. */
	if(chdir(argv[1]) == -1) {
		perror("chdir");
		exit(1);
	}
}
//直接複製hw3，這題似乎沒有要求
static void bi_echo(char **argv) {
	if(argv[1]==NULL){
		printf("at least one arguement for echo to copy.\n");
		return;
	}
	/* Fill in code. */
	int n,k=1;				//n代表要找的位置，k代表要查找或印出的命令列起始位置
	if(strcmp(argv[1],"-n")==0){
		n= atoi(argv[2]) -0 + 2 ;
		k+=2;
	}
	//printf("%ld\n%s\n",sizeof(argv),argv[sizeof(argv)]);
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

static void bi_hostname(char ** argv) {
	/* Fill in code. */
	char hostname[BUFFER_SIZE];
	if(gethostname(hostname,BUFFER_SIZE)==-1){
		printf("gethostname");
		exit(1);
	}
	printf("hostname: %s\n",hostname);
}

static void bi_id(char ** argv) {
 	/* Fill in code. */
	uid_t uid=getuid();
	struct passwd *pw;
	if( (pw=getpwuid(uid)) ==NULL){
		printf("getpwnam\n");
		exit(1);
	}
	printf("UserID: %d(%s), GroupID: %d(%s)\n",uid,pw->pw_name,pw->pw_gid,pw->pw_name);
}

static void bi_pwd(char ** argv) {
	/* Fill in code. */
	char buf[BUFFER_SIZE];
	if(getcwd(buf,BUFFER_SIZE)==NULL){
		printf("getpwd\n");
		exit(1);
	}
	printf("%s\n",buf);
}

static void bi_quit(char **argv) {
	exit(0);
}


/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; /* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {		//printf("%s\n",cmd);
	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
      		this = tableCommand;	//printf("%s\n",cmd);
      		return 1;
    	}
  return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
	this->do_it(argv);
}