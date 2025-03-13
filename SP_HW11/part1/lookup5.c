/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found;
	static int first_time = 1;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary.
		 * Fill in code. */
		if((fd = open(resource,O_RDONLY)) ==-1){
			perror("open\n");
			exit(1);
		}

		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);
		numrec = filsiz / sizeof(Dictrec);
		lseek(fd, 0, SEEK_SET);  // 重新將檔案指標移回文件開頭

		/* mmap the data.
		 * Fill in code. */
		if( (table=mmap(NULL,filsiz,PROT_READ,MAP_SHARED,fd,0)) == MAP_FAILED){
			perror("mmap\n");
			exit(1);
		}
		close(fd);
	}
    
	/* search table using bsearch
	 * Fill in code. */
	found = bsearch(sought->word, table, numrec, sizeof(Dictrec), dict_cmp);

	//if(found==NULL)	printf("No way, mf\n");
	//printf("lookup5: %s",sought->word);	傳來要找的word沒問題

	if (found) {
		strcpy(sought->text,found->text);
		// printf("FOUND lookup5: %s",sought->text);
		return FOUND;
	}

	// printf("lookup5 notfound\n");
	return NOTFOUND;
}
