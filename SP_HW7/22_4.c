#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);  //這行要加

static void handler(int sig){
    printf("Hello, I'm signal handler. MF!\n");
}
void SIG_HOLD(int sig){
    printf("sig%d was blocked before the call",sig);
}

sighandler_t mysigset(int sig,char disp[]){
    sigset_t block_set;
    if (sigprocmask(SIG_BLOCK, NULL, &block_set) == -1) {
        printf("sigprocmask in mysigset\n");   exit(1);
    }
    
    if(strcmp(disp,"SIG_DFL")==0){
        struct sigaction act;
        act.sa_handler = SIG_DFL;   //sa_handler是SIG_DFL和SIG_IGNORE以外的值時，才會解譯sa_mask和sa_flags (P.459頁)
        if(sigaction(sig, &act, NULL)==-1){
            printf("sigaction in mysigset\n");   exit(1);
        }
        printf("%d(sig_num) has been reset to default now\n",sig);
        pause();
    }else if(strcmp(disp,"SIG_IGNORE")==0){
        struct sigaction act;
        act.sa_handler = SIG_IGN;
        if(sigaction(sig, &act, NULL)==-1){
            printf("sigaction in mysigset\n");   exit(1);
        }
        printf("%d(sig_num) has been ignored now\n",sig);
        pause();
    }else if(strcmp(disp,"SIG_HOLD")==0){
        sigset_t hold_set;
        sigemptyset(&hold_set);
        sigaddset(&hold_set, sig);
        if(sigprocmask(SIG_BLOCK,&hold_set,NULL)==-1){
           printf("sigprocmask in mysigset\n");   exit(1);
        }
        printf("%d(sig_num) has been blocked now\n",sig);
        pause();
    }else return (sighandler_t)-1;

    if (sigismember(&block_set, sig)) {
        return (sighandler_t)SIG_HOLD;
    }else   return (sighandler_t)handler;

}
int mysighold(int sig){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    if(sigprocmask(SIG_BLOCK,&mask,NULL)==-1){
        printf("sigprocmask in mysighold\n");   return -1;       
    }
    return 0;
}
int mysigrelse(int sig){
    sigset_t del_sig_set;
    sigfillset(&del_sig_set);
    sigdelset(&del_sig_set,sig);
    if(sigprocmask(SIG_SETMASK,&del_sig_set,NULL)==-1){
        printf("sigprocmask in mysigrelse\n");   return -1;       
    }
    return 0;
}
int mysigignore(int sig){
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sigaction(sig, &act, NULL) == -1) {
        printf("sigaction in mysigignore\n");   return -1;
    }
    return 0;   
}

int mysigpause(int sig){
    sigset_t mask;
    sigemptyset(&mask); 
    sigaddset(&mask, sig); 
    return sigsuspend(&mask);
}

int main(int argc, char *argv[]){

    //<file> fun_name
    if(argc!=2){
        printf("Usage: <file> fun_name\n"); exit(1);
    }

    char fun[10];
    strcpy(fun,argv[1]);

    int sig;
    if(strcmp(fun,"sigset")==0){
        char disp[10];           
        printf("Please enter the sig_num (e.g. 2 for SIGINT):");   scanf("%d", &sig);    //使用SIGINT(2)
        printf("Please enter disp(SIG_DFL/SIG_IGNORE/SIG_HOLD):");   scanf("%s",disp);
        if(sig==9||sig==19){
            printf("The dispositions for SIGKILL and SIGSTOP cannot be changed.\n");    exit(1);
        }
        if(mysigset(sig,disp)==(sighandler_t)-1){
            printf("wrong disp[]\n");   exit(1);
        }
    }else if(strcmp(fun,"sighold")==0){
        printf("Please enter the sig_num (e.g. 2 for SIGINT) to add in in process's mask:");   scanf("%d", &sig);
        if(mysighold(sig)==-1){
            printf("mysighold\n");   exit(1);
        }
        printf("Successfully add sig%d in process's mask\n",sig);
        pause();
    }else if(strcmp(fun,"sigrelse")==0){
        printf("Please enter the sig_num (e.g. 2 for SIGINT) to been remove from process's mask:");    scanf("%d", &sig);
        if(mysigrelse(sig)==-1){
            printf("mysigrelse\n");   exit(1);
        }
        printf("Successfully remove sig%d from process's mask\n",sig);
        pause();
    }else if(strcmp(fun,"sigignore")==0){
        printf("Please enter the sig_num (e.g. 2 for SIGINT), which will be set to SIG_IGN:");   scanf("%d", &sig);
        if(mysigignore(sig)==-1){
            printf("mysigignore\n");   exit(1);
        }
        printf("Successfully set sig%d to SIG_IGN\n",sig);
        pause();
    }else if(strcmp(fun,"sigpause")==0){
        printf("Please enter the sig_num to block (e.g. 2 for SIGINT):");   scanf("%d", &sig);
        if(mysigpause(sig)==-1){
            printf("mysigpause\n");   exit(1);
        }
        printf("Successfully block sig%d\n",sig);
        pause();
    }

    return 0;
}