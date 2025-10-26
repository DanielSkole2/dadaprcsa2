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

    int test_status = 0;

    printf("\n--- fhistogram-mt Tests ---\n");

    printf("Test 1: Single file test (test1.txt). Output follows:\n");
    fflush(stdout);
    int status1 = system("./fhistogram-mt test1.txt");
    if (status1 == 0) {
        printf(" -> PASS: fhistogram-mt Single file test passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fhistogram-mt Single file test failed (Exit Status: %d).\n", status1);
        test_status = 1;
    }
    fflush(stdout);

    printf("\nTest 2: Multiple file test (3 files). Output follows:\n");
    fflush(stdout);
    int status2 = system("./fhistogram-mt test1.txt test2.txt test3.txt");
    if (status2 == 0) {
        printf(" -> PASS: fhistogram-mt Multiple file test passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fhistogram-mt Multiple file test failed (Exit Status: %d).\n", status2);
        test_status = 1;
    }
    fflush(stdout);

    printf("\nTest 3: Multithread test (-n 2). Output follows:\n");
    fflush(stdout);
    int status3 = system("./fhistogram-mt -n 2 test1.txt test2.txt test3.txt");
    if (status3 == 0) {
        printf(" -> PASS: fhistogram-mt multithread test passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fhistogram-mt multithread test failed (Exit Status: %d).\n", status3);
        test_status = 1;
    }
    fflush(stdout);

    printf("\n--- fhistogram-mt Tests finished ---\n\n");

    return test_status;
}
