#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#define MAX_SIZE 4096

int main(int argc,char *argv[]){
//由chatgpt得知可使用   sudo ls -l /proc/PID/fd/   和   sudo readlink /proc/1/fd/1  查看本題連結之所在位置和樣子
    if(argc>2){
        printf("too many arguements, only needs two\n");
        exit(1);
    }
        
    DIR *proc_dir;
    if( (proc_dir=opendir("/proc")) ==NULL){ //打開 /proc 目錄
        printf("opendir failead on '%s'\n","/proc");
        exit(1);
    }

    //readdir每次都會從參數(目錄指標)讀取下個目錄entry，並傳回一個靜態配置的dirent型別結構之指標    //課本p.389
    struct dirent *entry;
    errno=0;
    while( (entry=readdir(proc_dir)) !=NULL){
        
        if(entry->d_name[0]-'0'<0||entry->d_name[0]-'0'>9)  continue;       //因為/proc裡有像是version之類的目錄，其PID(entry->d_name)不是數字
        int PID=atoi(entry->d_name);
        char path[MAX_SIZE];
        snprintf(path, sizeof(path), "/proc/%d/fd", PID);
        //再打開目錄 /proc/PID/fd
        DIR *proc_dir2;
        if( (proc_dir2=opendir(path)) ==NULL){ //出現errno:13代表權限被拒，要加 sudo
            printf("opendir failead on '%s', errno: %d\n",path,errno);
            exit(1);
        }//printf("open success\n");

        struct dirent *entry2;
        errno=0;
        ssize_t numBytes;
        while( (entry2=readdir(proc_dir2)) !=NULL){
            int flag=0;

            if(entry2->d_name[0]-'0'<0||entry2->d_name[0]-'0'>9)  continue;       //排除/proc/PID/fd/n 裡非數字的檔案
            int n=atoi(entry2->d_name);
            char link_name[MAX_SIZE],buf[MAX_SIZE]={0};
            snprintf(link_name, sizeof(link_name), "/proc/%d/fd/%d", PID,n);    //readlink可直接從 lrwx------ 1 root root 64 10月 16 19:45 0 -> /dev/null 中提取 /dev/null
            if( (numBytes=readlink(link_name,buf,MAX_SIZE-1)) ==-1){
                printf("readlink,errno:%d",errno);
                exit(1);
            }

            if(strcmp(buf,argv[1])==0){//與要找的名稱相等，另外打開status檔案獲取Name資訊
                FILE *file2;
                char path2[MAX_SIZE];
                snprintf(path2, sizeof(path2), "/proc/%d/status", PID);
                if( (file2=fopen(path2,"r")) == NULL){
                    printf("failed to open %s",path2);
                    exit(1);
                }
                char line[MAX_SIZE],Name[MAX_SIZE];
                while(fgets(line,MAX_SIZE,file2)!=NULL){
                    if(strncmp(line,"Name:",5)==0){
                        strcpy(Name,line+6);
                        Name[strlen(Name)-1]='\0';
                        break;
                    }
                }
                printf("Pid: %d, Cmd Name: %s\n",PID,Name);
                if(fclose(file2)==EOF)   printf("close %s failed",path2);
            }

        }
        if(errno){
            printf("readdir2");
            exit(1);
        }
        if(closedir(proc_dir2)==-1){
            printf("failed to close '%s'\n",path);
            exit(1);
        }

    }
    if(errno){
        printf("readdir");
        exit(1);
    }
    if(closedir(proc_dir)==-1){
        printf("failed to close '/proc'\n");
        exit(1);
    }

    return 0;
}
