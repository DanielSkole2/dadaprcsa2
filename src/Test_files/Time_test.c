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

long gettimems() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

long getbytes(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return 0;
}

int main() {
    //creates input file for fhistogram
    FILE *fptr;
    fptr = fopen("Test_files/Inputs/speedfhtest.txt", "w");
    for (int i = 0; i < 1000; i++) {
        fprintf(fptr, "%d kylling\n", i);
    }
    fclose(fptr);

    //creates input file for fauxgrep
    FILE *fptr2;
    fptr2 = fopen("Test_files/Inputs/speedfgtest.txt", "w");
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
    system("./fhistogram Test_files/Inputs/speedfhtest.txt");
    int endtime = gettimems();
    printf("FH singlethread time is %d ms\n", endtime - starttime);
    printf("FH singlethread throughput is %f files per sec\n",(float)1.0/((endtime - starttime)/1000.0));
    printf("FH singlethread throughput is %f bytes per sec\n", (long)getbytes("Test_files/Inputs/speedfhtest.txt")/((endtime - starttime)/1000.0));


    //multithread test fhistogram
    starttime = gettimems();
    system("./fhistogram-mt -n 2 Test_files/Inputs/speedfhtest.txt");
    endtime = gettimems();
    printf("FH multithread time is %d ms\n", endtime - starttime);
    printf("FH multihread throughput is %f files per sec\n",(float)1.0/((endtime - starttime)/1000.0));
    printf("FH multithread throughput is %f bytes per sec\n", (long)getbytes("Test_files/Inputs/speedfhtest.txt")/((endtime - starttime)/1000.0));

    //singlethread test fauxgrep
    starttime = gettimems();
    system("./fauxgrep kylling Test_files/Inputs/speedfgtest.txt");
    endtime = gettimems();
    printf("FG singlethread time is %d ms\n", endtime - starttime);
    printf("FG singlethread throughput is %f files per sec\n",(float)1.0/((endtime - starttime)/1000.0));
    printf("FG singlethread throughput is %f bytes per sec\n", (long)getbytes("Test_files/Inputs/speedfgtest.txt")/((endtime - starttime)/1000.0));

    //multithread test fauxgrep
    starttime = gettimems();
    system("./fauxgrep-mt -n 2 kylling Test_files/Inputs/speedfgtest.txt");
    endtime = gettimems();
    printf("FG multithread time is %d ms\n", endtime - starttime);
    printf("FG multithread throughput is %f files per sec\n",(float)1.0/((endtime - starttime)/1000.0));
    printf("FG multithread throughput is %f bytes per sec\n", (long)getbytes("Test_files/Inputs/speedfgtest.txt")/((endtime - starttime)/1000.0));

    return 0;
}