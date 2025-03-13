/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;
//	printf("What?");
  	/* Nothing entered. */
  	if (line == NULL||strcmp(line,"\n")==0) {
    	return NULL;
  	}
//	printf("What?");
  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	if( (token=strtok(line,delim)) ==NULL)
		return NULL;
  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
//	printf("What?");
	if( (newArgv=malloc(1*sizeof(char*)))==NULL){
		perror("malloc newArgv failed");
		exit(1);
	}
	if( (newArgv[count]=malloc((strlen(token)+1)*sizeof(char))) == NULL){
		perror("malloc newArgv[0] failed");
		exit(1);
	}
	strcpy(newArgv[count++],token);
//	printf("%s ",newArgv[count-1]);
  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	while(token){
		token=strtok(NULL,delim);
		if(token==(char*)NULL)	break;		//這裡要加，不然會core dump
		if( (newArgv=realloc(newArgv,(count+1)*sizeof(char*)))==NULL){
			perror("realloc newArgv failed");
			exit(1);
		}
		if( (newArgv[count]=malloc((strlen(token)+1)*sizeof(char))) == NULL){
			printf("malloc newArgv[%d] failed",count);
			exit(1);
		}
		strcpy(newArgv[count++],token);
//		printf("%s ",newArgv[count-1]);
	}

//	printf("What?");
  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */
	if( (newArgv=realloc(newArgv,(count+1)*sizeof(char*)))==NULL){
		perror("realloc newArgv failed");
		exit(1);
	}
	if( (newArgv[count]=malloc(sizeof(NULL)*sizeof(char))) == NULL){
		printf("malloc newArgv[%d] failed",count);
		exit(1);
	}
//	printf("What?");
	newArgv[count]=(char*)NULL;
	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */
	while(oldArgv[i]){
		free(oldArgv[i++]);
	}
	free(oldArgv);
}