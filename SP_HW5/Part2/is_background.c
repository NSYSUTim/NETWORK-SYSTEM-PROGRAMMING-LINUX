/*
 * is_background.c :  check for & at end
 */
#include <string.h>
#include <stdio.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.
  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 *
	 * Fill in code.
	 */

	int i=0;
    while(myArgv[i]){
        if(strcmp(myArgv[i++],"&")==0){
            return TRUE;
        }
    }
	return FALSE;

}