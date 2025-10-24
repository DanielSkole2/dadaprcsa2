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

    if (system("./fhistogram-mt test1.txt") == 0) {
        printf("fhistogram Single file test passed\n");
    } 
    else {
        printf("fhistogram Single file test failed\n");
    }

    if (system("./fhistogram-mt test1.txt test2.txt test3.txt") == 0) {
        printf("fhistogram Multiple file test passed\n");
    }
    else {
        printf("fhistogram Multiple file test failed\n");
    }

    if (system("./fhistogram-mt -n 2 test1.txt test2.txt test3.txt") == 0) {
        printf("fhistogram multithread test passed\n");
    }
    else {
        printf("fhistogram multithread test failed\n");
    }

    return 0;
}