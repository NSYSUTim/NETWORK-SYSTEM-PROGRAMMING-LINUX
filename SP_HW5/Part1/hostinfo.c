#include <sys/utsname.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{


    char host_name[128];
    if( gethostname(host_name,128)==-1){
        perror("gethostname\n");
        exit(1);
    }printf("hostname: %s\n",host_name);

    struct utsname uts;
    if (uname(&uts) == -1)
        perror("uname");
    printf("%s\n", uts.release);

    long hid;
    if((hid=gethostid())==-1){
        perror("gethostid\n");
        exit(1);
    }
    printf("hostid: %ld\n",hid);

    exit(EXIT_SUCCESS);
}