//先執行./create_hole_file ，再執行此檔產生之執行檔./SP_hw1-2 hole_file copy_hole.txt

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

void perr_exit(const char *pe){
	perror(pe);
	exit(1);
	return;
}
int main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc > 3 )
        perr_exit("too many argument in cmd line");

    /* Open input and output files */

    if ( (inputFd = open(argv[1], O_RDONLY)) == -1)
        perr_exit("open");

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */

    if ( (outputFd = open(argv[2], openFlags, filePerms)) == -1)
        perr_exit("open");

    /* Transfer data until we encounter end of input or an error */

    char null_char[BUF_SIZE];
    if ((numRead = read(inputFd, buf, BUF_SIZE)) !=-1 ){//read一次讀取BUF_SIZE，回傳值大於0代表讀取到的bytes
        if(strcmp(buf , null_char)==0)   lseek(outputFd, numRead, SEEK_CUR);
        if (write(outputFd, buf, numRead) != numRead)
                perr_exit("write() returned error or partial write occurred");
    }else
        perr_exit("read");

    if (close(inputFd) == -1)
        perr_exit("close input");
    if (close(outputFd) == -1)
        perr_exit("close output");
	
    return 0;
}
