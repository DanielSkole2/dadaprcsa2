#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include "../job_queue.h"

int main() {
    //creating test data
    FILE *fptr;
    fptr = fopen("test1.txt", "w");
    fprintf(fptr, "this is test file 1\n");
    fclose(fptr);

    fptr = fopen("test2.txt", "w");
    fprintf(fptr, "this is test file 2\n");
    fclose(fptr);

    fptr = fopen("test3.txt", "w");
    fprintf(fptr, "this is test file 3\n");
    fclose(fptr);

    //starting tests
    if (system("./fhistogram-mt test1.txt") == 0) {
        printf("fhistogram-mt Single file test passed\n");
    } 
    else {
        printf("fhistogram-mt Single file test failed\n");
    }

    if (system("./fhistogram-mt test1.txt test2.txt test3.txt") == 0) {
        printf("fhistogram-mt Multiple file test passed\n");
    }
    else {
        printf("fhistogram-mt Multiple file test failed\n");
    }

    if (system("./fhistogram-mt -n 2 test1.txt test2.txt test3.txt") == 0) {
        printf("fhistogram-mt multithread test passed\n");
    }
    else {
        printf("fhistogram-mt multithread test failed\n");
    }

    return 0;
}