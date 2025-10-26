#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include "../job_queue.h"

int job_queue_init(struct job_queue *job_queue, int capacity);
int job_queue_push(struct job_queue *job_queue, void *data);
int job_queue_destroy(struct job_queue *job_queue);
int job_queue_pop(struct job_queue *job_queue, void **data);

int main() {
    int test_status = 0;
    
    printf("\n--- Job Queue (JQ) Tests ---\n");

    struct job_queue JQ;
    if (job_queue_init(&JQ, 10) == 0) {
        printf(" -> PASS: init passed\n");
        fflush(stdout);
        
        int data = 69;
        if (job_queue_push(&JQ, &data) == 0) {
            printf(" -> PASS: push passed\n");
            fflush(stdout);
        } else {
            printf(" -> FAIL: push failed\n");
            test_status = 1;
        }

        void *result;
        if (job_queue_pop(&JQ, &result) == 0) {
            printf(" -> PASS: pop passed\n");
            fflush(stdout);
        } else {
            printf(" -> FAIL: pop failed\n");
            test_status = 1;
        }

        job_queue_destroy(&JQ);
        printf("Job queue tests complete.\n");
        fflush(stdout);
    }
    else {
        printf(" -> FAIL: JQ init failed\n");
        test_status = 1;
    }
    
    printf("--- Job Queue Tests finished ---\n\n"); 
    
    return test_status;
}
