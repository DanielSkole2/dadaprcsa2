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


#define FH_TEST_FILE "speedfhtest.txt"
#define FG_TEST_FILE "speedfgtest.txt"

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
    FILE *fptr;
    fptr = fopen(FH_TEST_FILE, "w");
    if (fptr == NULL) {
        perror("Error opening FH_TEST_FILE");
        return 1;
    }
    for (int i = 0; i < 1000; i++) {
        fprintf(fptr, "%d kylling\n", i);
    }
    fclose(fptr);

    
    FILE *fptr2;
    fptr2 = fopen(FG_TEST_FILE, "w");
    if (fptr2 == NULL) {
        perror("Error opening FG_TEST_FILE");
        return 1;
    }
    for (int i = 0; i < 1000; i++) {
        if (i % 10 == 0) {
            fprintf(fptr2, "kylling\n");
        }
        else {
            fprintf(fptr2, "okse\n");
        }
    }
    fclose(fptr2);
    
    
    long fh_bytes = getbytes(FH_TEST_FILE);
    long fg_bytes = getbytes(FG_TEST_FILE);
    
    
    char command_buffer[512];
    
    
    printf("\n--- FHistogram Benchmarks ---\n");

    
    long starttime = gettimems();
    snprintf(command_buffer, sizeof(command_buffer), "./fhistogram %s", FH_TEST_FILE);
    system(command_buffer);
    long endtime = gettimems();
    int time_ms = endtime - starttime;

    printf("FH singlethread time is %d ms\n", time_ms);
    printf("FH singlethread throughput is %f files per sec\n",(float)1.0/((float)time_ms/1000.0));
    printf("FH singlethread throughput is %f bytes per sec\n", (float)fh_bytes/((float)time_ms/1000.0));


    
    starttime = gettimems();
    snprintf(command_buffer, sizeof(command_buffer), "./fhistogram-mt -n 2 %s", FH_TEST_FILE);
    system(command_buffer);
    endtime = gettimems();
    time_ms = endtime - starttime;

    printf("FH multithread time is %d ms\n", time_ms);
    printf("FH multihread throughput is %f files per sec\n",(float)1.0/((float)time_ms/1000.0));
    printf("FH multithread throughput is %f bytes per sec\n", (float)fh_bytes/((float)time_ms/1000.0));

    
    
    printf("\n--- Fauxgrep Benchmarks ---\n");

    
    starttime = gettimems();
    snprintf(command_buffer, sizeof(command_buffer), "./fauxgrep kylling %s", FG_TEST_FILE);
    system(command_buffer);
    endtime = gettimems();
    time_ms = endtime - starttime;

    printf("FG singlethread time is %d ms\n", time_ms);
    printf("FG singlethread throughput is %f files per sec\n",(float)1.0/((float)time_ms/1000.0));
    printf("FG singlethread throughput is %f bytes per sec\n", (float)fg_bytes/((float)time_ms/1000.0));

    
    starttime = gettimems();
    snprintf(command_buffer, sizeof(command_buffer), "./fauxgrep-mt -n 2 kylling %s", FG_TEST_FILE);
    system(command_buffer);
    endtime = gettimems();
    time_ms = endtime - starttime;

    printf("FG multithread time is %d ms\n", time_ms);
    printf("FG multithread throughput is %f files per sec\n",(float)1.0/((float)time_ms/1000.0));
    printf("FG multithread throughput is %f bytes per sec\n", (float)fg_bytes/((float)time_ms/1000.0));

    printf("\n--- Benchmarks Finished ---\n");

    return 0;
}