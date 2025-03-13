#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#define MAX_SIZE 4096
#define MAX_PID 128

typedef struct set          //原本要模擬set，用pid小到大排序，pid是唯一的，但太複雜沒用
{
    int pid,ppid;
    char name[MAX_SIZE];
    struct set *next;
}se;
typedef se *setptr;

setptr insert(setptr root,int ppid,int pid,char *name){//有insert就加到linked_list後面
    setptr newnode=(setptr)malloc(sizeof(se));

    newnode->pid=pid;
    newnode->ppid=ppid;
    strcpy(newnode->name,name);
    newnode->next=NULL;

    if(root==NULL){
        root=newnode;
    }else{
        setptr temp=root;
        while(temp->next!=NULL){
            temp=temp->next;
        }
        temp->next=newnode;
    }
    return root;
}

//要pri PID和Name
int f=0,j,l=0;//f: (flag決定 |—— or ————) ， l剩查看有無子行程之功能
//空白的格式不知道怎麼處理，採用觀察法，用pstree(1)顯示的比較，父子行程的關係正確
void dfs_show(setptr root,int ppi,int cur_dep){//一次印一個path，ppi(ppid), cur_dep(當前遞迴深度)
    setptr temp=root;
    while(temp!=NULL){
        if(temp->ppid==ppi){
            if(f==0)
                printf(" ———— %s, PID: %d",temp->name,temp->pid);
            else{
                //printf("cur_dep:%d",cur_dep);
                for(j=0;j<(cur_dep+1)*(6+6+7+3)-6;j++){
                    if(j==6+7+3){
                        printf("  |   ");   j+=6;
                    }else   printf(" ");
                }
                if(cur_dep==1)  for(j=0;j<8;j++) printf(" ");
                printf("  |—— %s, PID: %d",temp->name,temp->pid);
            }
            f=0;
            l=strlen(temp->name);
            dfs_show(root,temp->pid,cur_dep+1);   //需從頭再尋找，因為不確定pid和ppid的大小有無規則
        }
        temp=temp->next;
    }
    if(l==0)    return;
    f=1;            //printf("cur_dep:%d",cur_dep);
    printf("\n");
    l=0;
    return;
}

void free_set(setptr root){
    setptr temp=root;
    while(root!=NULL){
        root=root->next;
        free(temp);
        temp=root;
    }
    return;
}
//看chatgpt耗了很多時間
int main(int argc,char *argv[]){
//由上一題取得之Pid結果進行cat /proc/PID/status，發現使用PPid替換Pid一直搜尋下去，最後會是/proc/1/status，其PPid為0，而/proc/0/status無此目錄，且使用pstree(1)之輸出顯示之root為systemd，與/proc/1/status檔案之Name: systemd相同，故此題要求之init根節點 依據gpt說明為舊版Pid為1之行程之名稱)

    DIR *proc_dir;
    if( (proc_dir=opendir("/proc")) ==NULL){
        printf("opendir failed on '%s'\n","/proc");
    }
    struct dirent *entry;
    errno=0;
    setptr root=NULL;
    while( (entry=readdir(proc_dir)) !=NULL){   //printf("What's wrong with you?");

        if(entry->d_name[0]-'0'<0||entry->d_name[0]-'0'>9)  continue;
        int pid=atoi(entry->d_name),ppid=-1;
        FILE *file;
        char path[MAX_SIZE];
        snprintf(path,sizeof(path),"/proc/%d/status",pid);
        if( (file=fopen(path,"r")) ==NULL){
            printf("fopen failed on '%s': %d\n", path, errno);  //如果在開啟檔案之前，目錄消失了，則檔案無法成功開啟，往下一個目錄繼續
            continue;
        }
        char line[MAX_SIZE],Name[MAX_SIZE]={0};
        while( fgets(line,MAX_SIZE,file)!=NULL){
            if(strncmp(line,"PPid:",5)==0){
                ppid=atoi(line+5);
            }else if(strncmp(line,"Name:",5)==0){
                strcpy(Name,line+6);
                Name[strlen(Name)-1]='\0';
            }
            if(Name&&ppid!=-1) break;
        }
        if(fclose(file)==EOF)   printf("close %s failed",path);
        root=insert(root,ppid,pid,Name);    //printf("%s\n",root->name);
    }
    if(errno){
        printf("readdir, errno: %d",errno);
        exit(1);
    }
    if(closedir(proc_dir)==-1){
        printf("failed to close '/proc'\n");
        exit(1);
    }
    printf("%s, PID: %d ",root->name,root->pid);    //第一個找到的root剛好是根節點
    dfs_show(root,1,0);  //從pid為1開始，因為是所有行程一直找下去最後的父節點
    
    free_set(root);

    return 0;
}
