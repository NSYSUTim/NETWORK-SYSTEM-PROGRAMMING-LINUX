/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include "dict.h"
#define BIGLINE 512

int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	static FILE * in;
	static int first_time = 1;

	if (first_time) {
		first_time = 0;
		/* open up the file
		 *
		 * Fill in code. */
		if( (in=fopen(resource,"r")) ==NULL)	DIE("fopen\n");
	}

	/* read from top of file, looking for match
	 *
	 * Fill in code. */
	memset(sought->text,'\0',TEXT);
	char line[BIGLINE];
	rewind(in);
	while(!feof(in)) {
		/* Fill in code. */
		if(fgets(line,BIGLINE,in)==NULL){	if(feof(in))	break;	DIE("fgets\n");		}
		if(strncmp(line,sought->word,strlen(sought->word))==0 ){	//printf("Here\n");
			while(fgets(line,BIGLINE,in)!=NULL){
				if(line[0]=='\0'||line[0]=='\n')	break;
				strncat(sought->text,line,strlen(line));
			}
			//if(fclose(in)==EOF)	DIE("fclose\n");	//不能close因為只有開1次
			//printf("%ld\n%c",strlen(sought->text),sought->text[strlen(sought->text)-1]);
			sought->text[strlen(sought->text)-1]='\0';
			return FOUND;
		}
		//printf(line);
	}

	return NOTFOUND;
}
