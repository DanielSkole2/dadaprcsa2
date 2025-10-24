#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <sys/time.h>
#include "../job_queue.h"

int gettimems() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    //creates input file for fhistogram
    FILE *fptr;
    fptr = fopen("speedfhtest.txt", "w");
    for (int i = 0; i < 1000; i++) {
        fprintf(fptr, "%d kylling\n", i);
    }
    fclose(fptr);

    //creates input file for fauxgrep
    FILE *fptr2;
    fptr2 = fopen("speedfgtest.txt", "w");
    for (int i = 0; i < 1000; i++) {
        if (i % 10 == 0) {
            fprintf(fptr2, "kylling\n");
        }
        else {
            fprintf(fptr2, "okse\n");
        }
    }
    fclose(fptr2);

    //singlethread test fhistogram
    int starttime = gettimems();
    system("./fhistogram speedfhtest.txt");
    int endtime = gettimems();
    printf("FH singlethread time is %d ms\n", endtime - starttime);

    //multithread test fhistogram
    starttime = gettimems();
    system("./fhistogram-mt -n 2 speedfhtest.txt");
    endtime = gettimems();
    printf("FH multithread time is %d ms\n", endtime - starttime);

    //singlethread test fauxgrep
    starttime = gettimems();
    system("./fauxgrep kylling speedfgtest.txt");
    endtime = gettimems();
    printf("FG singlethread time is %d ms\n", endtime - starttime);

    //multithread test fauxgrep
    starttime = gettimems();
    system("./fauxgrep-mt -n 2 kylling speedfgtest.txt");
    endtime = gettimems();
    printf("FG singlethread time is %d ms\n", endtime - starttime);
    return 0;
}