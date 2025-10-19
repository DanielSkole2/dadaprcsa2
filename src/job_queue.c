#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>

#include "job_queue.h"

int job_queue_init(struct job_queue *job_queue, int capacity) {
    if (capacity <= 0) {
        return EINVAL;
    }

    job_queue->buffer = calloc(capacity, sizeof(void*));
    if (job_queue->buffer == NULL) {
        return ENOMEM;
    }

    job_queue->capacity = capacity;
    job_queue->head = 0;
    job_queue->tail = 0;
    job_queue->count = 0;
    job_queue->destroy_flag = false;

      
    if (pthread_mutex_init(&job_queue->lock, NULL) != 0) {
        free(job_queue->buffer);
        return -1; 
    }
    if (pthread_cond_init(&job_queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&job_queue->lock);
        free(job_queue->buffer);
        return -1;
    }
    if (pthread_cond_init(&job_queue->not_full, NULL) != 0) {
        pthread_cond_destroy(&job_queue->not_empty);
        pthread_mutex_destroy(&job_queue->lock);
        free(job_queue->buffer);
        return -1;
    }

    return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
    assert(pthread_mutex_lock(&job_queue->lock) == 0);

    
    job_queue->destroy_flag = true;

    
    while (job_queue->count > 0) {
        
        assert(pthread_cond_wait(&job_queue->not_empty, &job_queue->lock) == 0);
    }

    
    
    assert(pthread_cond_broadcast(&job_queue->not_empty) == 0);

    assert(pthread_mutex_unlock(&job_queue->lock) == 0);

    
    assert(pthread_mutex_destroy(&job_queue->lock) == 0);
    assert(pthread_cond_destroy(&job_queue->not_empty) == 0);
    assert(pthread_cond_destroy(&job_queue->not_full) == 0);
    free(job_queue->buffer);

    return 0;
}

int job_queue_push(struct job_queue *job_queue, void *data) {
    assert(pthread_mutex_lock(&job_queue->lock) == 0);

    
    if (job_queue->destroy_flag) {
        assert(pthread_mutex_unlock(&job_queue->lock) == 0);
        return -1; 
    }

    
    while (job_queue->count == job_queue->capacity) {
        assert(pthread_cond_wait(&job_queue->not_full, &job_queue->lock) == 0);
        
        if (job_queue->destroy_flag) {
            assert(pthread_mutex_unlock(&job_queue->lock) == 0);
            return -1;
        }
    }

    
    job_queue->buffer[job_queue->tail] = data;
    job_queue->tail = (job_queue->tail + 1) % job_queue->capacity;
    job_queue->count++;

    
    assert(pthread_cond_signal(&job_queue->not_empty) == 0);

    assert(pthread_mutex_unlock(&job_queue->lock) == 0);
    return 0;
}

int job_queue_pop(struct job_queue *job_queue, void **data) {
    assert(pthread_mutex_lock(&job_queue->lock) == 0);

    
    while (job_queue->count == 0) {
        if (job_queue->destroy_flag) {
            
            assert(pthread_mutex_unlock(&job_queue->lock) == 0);
            return -1;
        }
        
        assert(pthread_cond_wait(&job_queue->not_empty, &job_queue->lock) == 0);
    }

    

    
    if (job_queue->count == 0 && job_queue->destroy_flag) {
        assert(pthread_mutex_unlock(&job_queue->lock) == 0);
        return -1;
    }

    
    *data = job_queue->buffer[job_queue->head];
    job_queue->head = (job_queue->head + 1) % job_queue->capacity;
    job_queue->count--;

    
    assert(pthread_cond_signal(&job_queue->not_full) == 0);

    assert(pthread_mutex_unlock(&job_queue->lock) == 0);

    
    if (job_queue->count == 0 && job_queue->destroy_flag) {
        assert(pthread_cond_signal(&job_queue->not_empty) == 0);
    }


    return 0; 
}
