/*
 * parse.c : use whitespace(空格) to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	token = strtok(line,delim);
	if(token==NULL)	return NULL;

  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
	if( (newArgv=malloc( 1* sizeof(char*))) == NULL){
		perror("malloc newArgv failed");
		exit(1);
	}
	//sizeof 包含'\0'，strlen不包含'\0'
	if( (newArgv[count]=malloc(strlen(token)*sizeof(char))) == NULL){
		perror("malloc newArgv[0] failed");
		exit(1);
	}
	printf("[%d] : %s\n",count,token);
	strcpy(newArgv[count++],token);		//C 字串不能直接用等號指定 !

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
		if(token==(char*)NULL)	break;
		if( (newArgv = realloc(newArgv,(count+1)*sizeof(char*))) == NULL){
			perror("realloc newArgv failed");
			exit(1);
		}
		if( (newArgv[count] = malloc( (strlen(token)+1 )* sizeof(char))) == NULL){
			printf("malloc newArgv[%d] failed",count);
			exit(1);
		}
		strcpy(newArgv[count++],token);		//printf("%s\n",token);
		printf("[%d] : %s\n",count-1,newArgv[count-1]);
	}

  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */
	if( (newArgv = realloc(newArgv,(count+1)*sizeof(char*))) == NULL){
		perror("realloc newArgv failed");
		exit(1);
	}
	if( (newArgv[count] = malloc( sizeof(NULL) * sizeof(char))) == NULL){
		printf("malloc newArgv[%d] failed",count);
		exit(1);
	}
	newArgv[count]=(char*)NULL;		//要用NULL，'\0'會錯，是個一維陣列，然後 NULL 不能輸出
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
	for(i; oldArgv[i]!=NULL ;i++)	free(oldArgv[i]);
	free(oldArgv);
}

