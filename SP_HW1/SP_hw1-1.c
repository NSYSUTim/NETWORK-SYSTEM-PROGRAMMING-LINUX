#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 1024
#define BUFFER_SIZE 10240 // 可儲存多次輸入的緩衝區大小

void perr_exit(const char *pe) {
    perror(pe);
    exit(1);
    return;
}

int main(int argc, char *argv[]) {
    int get_opt, append = 0;
    while ((get_opt = getopt(argc, argv, "+a")) != -1) {
        switch (get_opt) { 
            case 'a': 
                append = 1; 
                break;
        }
    }
    
    char all_input[BUFFER_SIZE] = {0};  // 用來儲存所有輸入的緩衝區
    ssize_t total_size = 0;

    while (1) {
        char from_in[MAX_SIZE];
        ssize_t read_size;
        
        if ((read_size = read(STDIN_FILENO, from_in, MAX_SIZE)) == -1) {
            perr_exit("read");
        } else if (read_size == 0) {
            break;  // 結束讀取（例如收到 EOF)
        }
        
        // 將讀取的輸入內容複製到 all_input 緩衝區
        if (total_size + read_size >= BUFFER_SIZE) {
            fprintf(stderr, "Buffer overflow, exiting.\n");
            exit(1);
        }
        
        memcpy(all_input + total_size, from_in, read_size);
        total_size += read_size;
        
        // 寫入 stdout 顯示當前的輸入
        if (write(STDOUT_FILENO, from_in, read_size) == -1) {
            perr_exit("write to stdout");
        }
    //}

    // 設定 open() flags
    int flags = (append) ? (O_APPEND | O_WRONLY | O_CREAT) : (O_TRUNC | O_WRONLY | O_CREAT);
    mode_t mode = S_IRUSR | S_IWUSR;
    
    // 開啟並寫入每一個指定的檔案
    int fd, i;
    for (i = optind; i < argc; i++) {
        fd = open(argv[i], flags, mode);
        if (fd == -1) {
            perr_exit("open");
        }
        
        if (write(fd, all_input, total_size) == -1) {
            perr_exit("write to fd");
        }
        
        if (close(fd) == -1) {
            perr_exit("close");
        }
    }
	}
    return 0;
}
