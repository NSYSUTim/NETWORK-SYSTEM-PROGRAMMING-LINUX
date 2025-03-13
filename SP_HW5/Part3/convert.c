/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr, blank;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	//w沒有的話會創造一個
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */

		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */
		if(fgets(line,BIGLINE,in)==NULL)	DIE("fgets\n");
		strncpy(blank.word,line,WORD);
		blank.word[strlen(blank.word)]='\0';
		//printf("WHAT\n");
		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */
		int i;
		while(fgets(line,BIGLINE,in)!=NULL){
			if (line[0]=='\n')	break;
			strncat(blank.text,line,TEXT);
			blank.text[strlen(blank.text)]='\0';
		}
		blank.text[strlen(blank.text)]='\n';
		//printf("WHAT\n");
		/* Write record out to file.
		 *
		 * Fill in code. */
		if(fwrite(blank.word,strlen(blank.word),1,out)<0)	DIE("fwrite word\n");
		if(fwrite(blank.text,strlen(blank.text),1,out)<0)	DIE("fwrite text\n");
		//printf("WHAT\n");

		memset(blank.text, '\0', TEXT);
		memset(blank.word, '\0', WORD);
	}

	fclose(in);
	fclose(out);
	return 0;
}
