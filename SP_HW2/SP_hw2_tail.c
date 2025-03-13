//複習 : getopt 處理 -a -b 的自定義 option
// 利用緩衝區而非頻繁的系統呼叫較佳
// O_SYNC 同步寫檔，會頻繁系統呼叫，影響性能
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFF_SIZE 4096 //p.257提到此大小幾乎達到最佳效能

void perr_exit(const char *pe){
	printf("%s",pe);
	exit(1);
	return;
}

int main(int argc,char *argv[]){

	//If such a character is followed by a colon(:),  the  option requires an argument
	int fd,opt,num=10,n=1;
	while( (opt = getopt(argc,argv,"n:") ) !=-1){
		switch(opt){
			case 'n':	num = atoi(optarg);	n=3;
		}
	}
	
	char buf[BUFF_SIZE];
	if( setvbuf(stdout,buf,_IOFBF,BUFF_SIZE) !=0 )	perr_exit("setvbuf");	//default設置，以緩衝區大小進行讀寫

	off_t offset=0, len=0;
	if( posix_fadvise( fd, offset, len, POSIX_FADV_NORMAL ) !=0)	perr_exit("posix_fadvise");	//default設置	
	
	if( (fd=open(argv[n],O_RDONLY)) ==-1)	//O_CREATE 才要 mode參數
		perr_exit("failed to open \"file\"");
	
	off_t read_size, coun=0;
	if( (read_size = read(fd, buf, BUFF_SIZE)) ==-1)	perr_exit("file_size unfound");
	buf[read_size] = '\0';

	int i;
	for(i=read_size-1;i>=0;i--){
		if( buf[i] == '\n')	coun++;
		if(coun==num+1){
			offset = read_size - i ;
			break;
		}
	}
	//printf("%ld %d\n",coun,num);
	
	if(coun==num)	offset = read_size+1;	//如果為整個檔案都要輸出
	else if(coun<num)	perr_exit("num is too big");
	//printf("offset = %ld\nread_size = %ld\n%c", offset,read_size,buf[read_size] );
	//這裡的offset是要讀取的bytes
	if( write( STDOUT_FILENO, buf + read_size - offset + 1 , offset-1 ) ==-1)	perr_exit("write"); 
	
	if(close(fd)==-1) perr_exit("close");

	return 0;
}
