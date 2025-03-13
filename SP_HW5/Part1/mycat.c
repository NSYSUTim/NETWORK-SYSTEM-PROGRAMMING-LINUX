#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[]) {

    if(argc!=2){
        printf("Usage: mycat filename\n");
        exit(1);
    }
    FILE *fd;
    if( (fd=fopen(argv[1],"r")) ==NULL){
        printf("fopen");
        exit(1);
    }
    char line[1024];
    while( fgets(line,1024,fd) !=NULL){
        printf("%s",line);
    }
    if(fclose(fd)==EOF){
        printf("close");
        exit(1);
    }


    return 0;
}