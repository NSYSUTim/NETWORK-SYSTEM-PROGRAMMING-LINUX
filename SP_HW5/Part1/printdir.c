#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char *buf = NULL;
    size_t size = 0;

    if ( (buf=getcwd(NULL, 0)) == NULL) {
        perror("getcwd");
        return 1;
    }
    printf("%s\n", buf);

    free(buf);
    return 0;
}