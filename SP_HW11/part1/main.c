/*
 * The same main will do to test all the versions  of  lookup.   The
 * argument  to  main  is  always  passed  as the second argument to
 * lookup. It identifies the resource needed by lookup to  find  its
 * answers.  It  might be a file name or a named pipe, or the key to
 * some shared memory or a message queue, etc. The point is,  it  is
 * just  a  string;  main does not need to understand how the string
 * will be used. The work involved in each  exercise  is  to  define
 * lookup in different ways. What is the same throughout is that the
 * FIRST time lookup is called, it may have to open some resource. A
 * static internal flag is used to determine whether or not this was
 * the first call.
 */

// 可用 strings fixrec > output.txt 查看 2 進制的檔案

#include <errno.h>

#include "dict.h"

int main(int argc, char **argv) {
	FILE *in;
	char word[WORD];
	Dictrec tryit;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <resource>\n",argv[0]);
		exit(errno);
	}

	while(fputs("What word do you want : ",stderr),fgets(tryit.word, sizeof(tryit.word), stdin)) {	//gets(tryit.word)
		//printf("main: %s\n",tryit.word);
		tryit.word[strlen(tryit.word)-1]='\0';		//換行符要去掉!，不然找不到
		switch(lookup(&tryit,argv[1]) ) {
			case FOUND:
				// printf("main FOUND");
				printf("%s : %s",tryit.word,tryit.text);
				break;
			case NOTFOUND:
				// printf("main NOTFOUND");
				printf("%s : Not Found!\n",tryit.word) ;
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
		// fflush(stdout);
		//sleep(1);
	}
}
