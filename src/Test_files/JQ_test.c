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
    struct job_queue JQ;
    if (job_queue_init(&JQ, 10) == 0) {
        printf("init passed\n");
    
        int data = 69;
        if (job_queue_push(&JQ, &data) == 0) {
            printf("push passed\n");
        }

        void *result;
        if (job_queue_pop(&JQ, &result) == 0) {
            printf("pop passed\n");
        }

        job_queue_destroy(&JQ);
        printf("Job queue passed\n");
    }
    else {
        printf("JQ failed\n");
    }
    return 0;
}