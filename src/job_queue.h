#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <pthread.h>
#include <stdbool.h>

struct job_queue {
    void **buffer;      
    int capacity;       
    int head;           
    int tail;           
    int count;          
    bool destroy_flag;  

    
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};


int job_queue_init(struct job_queue *job_queue, int capacity);

int job_queue_destroy(struct job_queue *job_queue);

int job_queue_push(struct job_queue *job_queue, void *data);

int job_queue_pop(struct job_queue *job_queue, void **data);

#endif
