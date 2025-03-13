//  ./44_2 ls r
//  ./44_2 cat w
//  gpt產生之測試
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_POPEN 128         // 可以同時打開的 popen 數量
#define CMD_SIZE 256          // 最大命令長度

FILE *popen_streams[MAX_POPEN]; // 儲存 popen 的 FILE *流
pid_t popen_pids[MAX_POPEN];     // 儲存對應的子進程ID
int popen_count = 0;              // 當前已開啟的 popen 數量

FILE *my_popen(const char *command, const char *mode) {
    // 檢查是否超過同時打開的最大數量，但如果像本題的main裡一樣popen一次就pclose一次，就不會有這個問題
    if (popen_count >= MAX_POPEN) {
        printf("Error: reached maximum popen limit.\n");
        return NULL;
    }
    
    int pfd[2];
    pid_t pid;
    FILE *stream = NULL;

    if (strcmp(mode, "r") == 0) {   //父行程從子行程read
        if (pipe(pfd) == -1) {
            perror("pipe");
            return NULL;
        }
        
        switch (pid = fork()) {
            case -1:
                perror("fork");
                return NULL;

            //popen會建立一個執行command指令的子行程(p.994)
            case 0: // child的標準輸出重新導向到pipe的寫入端
                close(pfd[0]);//pipe不讀取
                dup2(pfd[1], STDOUT_FILENO); // 將子行程標準輸出重定向到管道寫入端，dup2重新定向
                close(pfd[1]);
                // 關閉已經開啟的file stream
                for (int i = 0; i < popen_count; i++) {
                    close(fileno(popen_streams[i]));
                }
                execl("/bin/sh", "sh", "-c", command, (char *)NULL); // 執行命令
                perror("execl");
                exit(1);

            default: // parent從pipe裡讀取
                close(pfd[1]);//pipe不用write端
                stream = fdopen(pfd[0], "r"); // pfd是int型別的file descriptor，要用fdopen改成FILE *型別
                //不用dup2是因為popen是要回傳FILE*，供後續使用

                popen_streams[popen_count] = stream;
                popen_pids[popen_count++] = pid;
                //不關閉pfd[0]因為還有下一個要繼續用
        }
    } else if (strcmp(mode, "w") == 0) {    //父行程write到子行程
        if (pipe(pfd) == -1) {
            perror("pipe");
            return NULL;
        }
        
        switch (pid = fork()) {
            case -1:
                perror("fork");
                return NULL;

            case 0: // child的標準輸入變成pipe的讀取端
                close(pfd[1]);
                dup2(pfd[0], STDIN_FILENO); // 將標準輸入重定向到管道讀取端
                close(pfd[0]);

                // 要關閉已經開啟的file stream
                for (int i = 0; i < popen_count; i++) {
                    close(fileno(popen_streams[i]));    //因為close要接受整數行別的參數。所以用fileno，為fdopen的反向
                }
                execl("/bin/sh", "sh", "-c", command, (char *)NULL); // 執行命令
                perror("execl");
                exit(1);

            default: // parent
                close(pfd[0]);
                stream = fdopen(pfd[1], "w"); // 取得寫入流

                popen_streams[popen_count] = stream;
                popen_pids[popen_count++] = pid;
        }
    } else {
        printf("wrong mode\n");
        return NULL;
    }

    return stream;
}

int my_pclose(FILE *stream) {
    for (int i = 0; i < popen_count; i++) {
        if (popen_streams[i] == stream) {
            pid_t pid = popen_pids[i];
            int fd = fileno(stream);
            close(fd);
            // 清除數據結構
            popen_streams[i] = NULL;
            popen_pids[i] = 0;

            int stat;
            if (waitpid(pid, &stat, 0) == -1) { // 等待子進程結束
                perror("waitpid");
                return -1; // 如果有錯誤則返回-1
            }
            if(WIFEXITED(stat)){
                printf("exited, status=%d\n", WEXITSTATUS(stat));
            }else if (WIFSIGNALED(stat)){
                printf("killed by signal %d\n", WTERMSIG(stat));
            }else if (WIFSTOPPED(stat)){
                printf("stopped by signal %d\n", WSTOPSIG(stat));
            }
            popen_count--;
            return stat;
        }
    }
    return -1; // 找不到對應的流
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: <command> <mode>\n");
        exit(1);
    }

    FILE *fp = my_popen(argv[1], argv[2]);
    if (fp == NULL) {
        printf("Failed to open process.\n");
        exit(1);
    }

    if (strcmp(argv[2], "r") == 0) {
       //父行程從子行程read
        char buf[256];
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            if(write(STDOUT_FILENO, buf, strlen(buf)) == -1){
                perror("write");
                exit(1);
            }
        }
    } else if (strcmp(argv[2], "w") == 0) {
       //父行程write到子行程
        char input[256];
        printf("Enter for command: ");
        if (fgets(input, sizeof(input), stdin) == NULL) { // 從標準輸入讀取
            perror("fgets");
            exit(1);
        }
        if (write(fileno(fp), input, strlen(input)) == -1) { // 寫入輸入
            perror("write");
            exit(1);
        }
    }

    if(my_pclose(fp)==-1){
        printf("my_pclose");
        exit(1);
    }
    return 0;
}
