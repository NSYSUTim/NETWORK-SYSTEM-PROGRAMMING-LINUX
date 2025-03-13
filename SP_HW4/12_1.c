#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
//256太小
#define MAX_SIZE 4096

//直接使用課本範例
uid_t           /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */
        return -1;                      /* return an error */

    u = strtol(name, &endptr, 10);      /* As a convenience to caller */
    if (*endptr == '\0')                /* allow a numeric string */
        return u;

    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;

    return pwd->pw_uid;
}

int main(int argc,char *argv[]){
//先使用課本所示之 cat /proc/1/status，可知其中一個process的Pid是1，Uid是0 0 0 0，Name是systemd
    if(argc>2){
        printf("too many arguements, only needs two\n");
        exit(1);
    }
    
    uid_t user_id=userIdFromName(argv[1]);  //取得UserID
    
    DIR *proc_dir;
    if( (proc_dir=opendir("/proc")) ==NULL){ //打開 /proc 目錄
        printf("opendir failed on '%s'\n","/proc");
        exit(1);
    }

    //readdir每次都會從參數(目錄指標)讀取下個目錄entry，並傳回一個靜態配置的dirent型別結構之指標    //課本p.389
    struct dirent *entry;
    errno=0;
    while( (entry=readdir(proc_dir)) !=NULL){
        
        if(entry->d_name[0]-'0'<0||entry->d_name[0]-'0'>9)  continue;       //因為/proc裡有像是version之類的目錄，其PID(entry->d_name)不是數字
        int PID=atoi(entry->d_name);
        FILE *file;
        char path[MAX_SIZE];
        snprintf(path, sizeof(path), "/proc/%d/status", PID);
        if( (file=fopen(path,"r")) == NULL){
            continue;       //如果在開啟檔案之前，目錄消失了，則檔案無法成功開啟，往下一個目錄繼續
        }

        char Name[MAX_SIZE]={0},Uid[MAX_SIZE]={0};  //char TestPid[MAX_SIZE]={0};
        uid_t uid=-1;
        //read 只能讀取成字串，這樣只能再用字元遍歷，不方便
        // if( read(fd,line,MAX_SIZE) ==-1){
        //     printf("read %s failed",entry->name);
        //     exit(1);
        // }
        char line[MAX_SIZE];
        while(fgets(line,MAX_SIZE,file)!=NULL){
            if(strncmp(line,"Name:",5)==0){ //可執行 cat /proc/1/status，則知道其中一個不一定是要找的檔案的形式
                strcpy(Name, line+6);   //因為line是陣列，指針指向第一項，+5之後可以越過Name:取得指令名稱
                //printf("Name = %s\n", Name);
            }
            else if(strncmp(line,"Uid:",4)==0){
                strcpy(Uid, line+4);
                //printf("Uid: %s\n", Uid);//印出此/proc/PID目錄status檔案的Uid:後面的內容，可知道可以輸入甚麼使用者名稱UserID測試
                uid=atoi(Uid);      //會轉換字串裡第一個數字，Uid:後面四碼似乎都一樣 Real, effective, saved set, and FS UIDs，Real UserID是執行行程之使用者ID，應為本題之要求
            }
            //  /proc/PID/status檔案裡的Pid就是他的PID，所以不用再提取
            // else if(strncmp(line,"Pid:",4)==0){
            //     strcpy(TestPid, line+4);
            //     printf("TestPid: %s\nPID:%d", TestPid,PID);
            // }
            if(Name[0]&&Uid[0])   break;
        }
        if(fclose(file)==EOF)   printf("close %s failed",path);
   
        if(uid==user_id){     //如果User ID和輸入的相同
            printf("PID: %d\nName: %s\n",PID,Name); //印出此行程檔 之其PID和名稱
        }
    }
    if(errno){//如errno不等於0，代表readdir途中有錯
        printf("readdir");
        exit(1);
    }
    if(closedir(proc_dir)==-1){
        printf("failed to close '/proc'\n");
        exit(1);
    }

    return 0;
}
