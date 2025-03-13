#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define FIFO_FILE "44_7_fifo_file"
#define test_size 256
int main(){
//測試open NONBLOCK時，write() / read()的使用情況
    int rfd,wfd;
    char write_msg[]="Test msg is coming, watch out. MF!\n\n";
    char read_msg[test_size];
    ssize_t readbytes;

    // 如已存在，刪除 FIFO 檔案
    if (remove(FIFO_FILE) == -1 && errno != ENOENT) {  // != ENOENT 檔案 (!)沒有 不存在，代表沒刪除且檔案存在時才是函是呼叫失敗
        printf("remove %s failed",FIFO_FILE);
        exit(1);
    }

    if(mkfifo(FIFO_FILE,0666)==-1){      //mode決定權限
        printf("mkfifo %s", FIFO_FILE);
        exit(1);
    }printf("make %s success\n\n",FIFO_FILE);
    
    //先開FIFO檔案的寫端時，如果讀端還沒開啟，會錯誤。 課本P.1009
    if( (wfd=open(FIFO_FILE, O_WRONLY | O_NONBLOCK )) ==-1){
        printf("Read end of %s is closed now. Failed to open write end.\n\n",FIFO_FILE);
    }
    if((rfd=open(FIFO_FILE, O_RDONLY | O_NONBLOCK )) ==-1){
        printf("Failed to open read end of %s",FIFO_FILE);
        exit(1);
    }
    if((wfd=open(FIFO_FILE, O_WRONLY | O_NONBLOCK )) ==-1){
        printf("Failed to open write end of %s",FIFO_FILE);
        exit(1);
    }
    printf("read / write end of %s is opened with NONBLOCK flags now.\n\n",FIFO_FILE);
    //read()或write()另一端還沒開啟時，無論有無NONBLOCK旗幟皆會錯誤，故不測試

    //非阻塞式read()函數測試
    //對照課本表格，有啟用O_NONBLOCK，write end開啟，0 bytes(FIFO中無資料)時，非阻塞式read會失敗。無啟用則會阻塞，非本題測試之項目
    if(read(rfd,read_msg,test_size)==-1){
        printf("Read failed: no data in %s yet\n\n",FIFO_FILE);
    }
    //FIFO已有資料則可以成功讀取，非阻塞式write到FIFO還沒滿時基本上正常執行
    if(write(wfd,write_msg,strlen(write_msg))==-1){
        printf("Failed to write %s into %s",write_msg,FIFO_FILE);
        exit(1);
    }
    if((readbytes=read(rfd,read_msg,test_size)) ==-1){
        printf("Failed to read from %s",FIFO_FILE);
        exit(1);
    }
    if(write(STDOUT_FILENO,read_msg,readbytes)==-1){
        printf("Failed to write %s",read_msg);
        exit(1);
    }

    //非阻塞式write()函數測試
    //啟用O_NONBLOCK，read端開啟時，write有兩種情形 1. FIFO未寫滿時(如前)   2. FIFO寫滿時
    while(write(wfd,write_msg,strlen(write_msg)) !=-1){}
    
    printf("write() don't wait for FIFO to have space to write in under NONBLOCK. If no space to write in(%s is full), it fails.\n\n",FIFO_FILE);

    if(close(wfd)==-1){
        printf("close wfd");
        exit(1);
    }
    if(close(rfd)==-1){
        printf("close rfd");
        exit(1);
    }

    return 0;
}
