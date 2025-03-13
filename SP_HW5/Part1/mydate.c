#include <sys/types.h>
#include <time.h>
#include <stdio.h>

int main(){

    time_t t;
    struct tm *tmp;
    char timestr[40];

    time(&t);
    tmp = localtime(&t);
    if (strftime(timestr, sizeof(timestr), "%b %d (%a), %Y %I:%M %p", tmp) == 0) {
        printf("strftime returned 0");
        return 1;
    }
    printf("%s\n",timestr);

    return 0;
}