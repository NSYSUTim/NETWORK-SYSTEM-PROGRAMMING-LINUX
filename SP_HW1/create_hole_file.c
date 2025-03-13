#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 1024
#define HOLE_SIZE 5

void perr_exit(const char *message){
        perror("message");
        exit(1);
}

int main(void){
        char content[FILE_SIZE];
        int i, fd;

        printf("Enter content for hole_file: \n");
        fgets(content, FILE_SIZE, stdin);	//一次讀取一整行用fgets

        //S_IWUSR : user has write permisiion, S_IRUSR : user has read permission
        if( (fd = open("hole_file", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR)) == -1)
		perr_exit("failed to open hole file");

        for(i = 0; i < strlen(content); i++){
 		if( (write(fd, &content[i], sizeof(char))) == -1)
                        perr_exit("writing error");
                lseek(fd, HOLE_SIZE, SEEK_CUR);	//每寫入一個字元就新增空洞(把檔案指標(偏移量)往後移hole_size(自動編譯為off_t))
        }

        if(close(fd) == -1)
                perr_exit("close");
        printf("\"hole_file\" is created successfully\n");

        return 0;
}

