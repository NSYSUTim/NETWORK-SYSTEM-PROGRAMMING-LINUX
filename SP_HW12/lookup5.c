#include <stdlib.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/mman.h>
#include <string.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a, const void *b) {
	return strcmp((char *)a, ((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec *table;
	static int numrec;
	Dictrec *found, temp;
	static int first_time = 1;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary. */
		if ((fd = open(resource, O_RDONLY)) == -1)
			DIE("open");

		/* Get record count for building the tree. */
		filsiz = lseek(fd, 0L, SEEK_END);
		numrec = filsiz / sizeof(Dictrec);

		/* mmap the data. */
		table = (Dictrec *)mmap(NULL, filsiz, PROT_READ, MAP_PRIVATE, fd, 0);
		if (table == MAP_FAILED)
			DIE("mmap");

		close(fd);
	}

	/* Search table using bsearch. */
	strcpy(temp.word, sought->word);
	found = (Dictrec *)bsearch(&temp, table, numrec, sizeof(Dictrec), dict_cmp);

	if (found) {
		strcpy(sought->text, found->text);
		return FOUND;
	}

	return NOTFOUND;
}

