#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// mapping 是把檔案內容複製到記憶體，這樣操作記憶體就可以直接操作到檔案內容
// MAP_SHARED(public) mapping: 修改映射內容會一起修改到原本的檔案，MAP_PRIVATE(私有映射)則不會
// mapping 是雙向的，也可以從記憶體映射到另一個檔案


int main(int argc,char *argv[]){
//dest_file 和 執行檔 都要用sudo才可以
    void *src_addr,*dest_addr;
    struct stat src_stat;
    int src_fd,dest_fd;
    off_t src_size;

    if(argc!=3){
        printf("Usage: %s <source_file> <dest_file>\n",argv[0]);
        exit(1);
    }

    if((src_fd=open(argv[1],O_RDONLY)) ==-1){
        perror("open src\n"); exit(1);
    }
    if(fstat(src_fd,&src_stat)==-1){    //
        perror("fstat\n"); exit(1);
    }
    src_size=src_stat.st_size;    //取得輸入檔案大小
    if((src_addr=mmap(NULL,src_size,PROT_READ,MAP_PRIVATE,src_fd,0)) ==(void *)-1){//用private映射才不會更動到原檔案
        perror("mmap src_addr\n"); exit(1);
    }

    //即使只要寫入dest這裡也要把檔案的讀寫模式都開啟，不然會錯
    if((dest_fd=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,src_stat.st_mode)) ==-1){ //O_TRUNC只有在write模式時會清空檔案內容
        perror("open dest\n"); exit(1);
    }
    if(ftruncate(dest_fd,src_size)==-1){ //設定輸出檔案大小
        perror("ftruncate\n"); exit(1);
    }
    if((dest_addr=mmap(NULL,src_size,PROT_WRITE,MAP_SHARED,dest_fd,0)) ==(void *)-1){//用MAP_SHARE才可以更改dest_file
        perror("mmap dest_addr\n"); exit(1);
    }

    memcpy(dest_addr,src_addr,src_size);
    
    printf("%s has been successfully copyed to %s using memmory mapping.\n",argv[1],argv[2]);
    
    if(close(src_fd)==-1){
        perror("close src_fd\n"); exit(1);
    }
    if(close(dest_fd)==-1){
        perror("close src_fd\n"); exit(1);
    }
    if(munmap(src_addr,src_size)==-1){
        perror("munmap src_fd\n"); exit(1);
    }
    if(munmap(dest_addr,src_size)==-1){
        perror("munmap dest_fd\n"); exit(1);
    }
    return 0;
}