// Setting _DEFAULT_SOURCE is necessary to activate visibility of
// certain header file contents on GNU/Linux systems.
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include "job_queue.h"

pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t histogram_mutex = PTHREAD_MUTEX_INITIALIZER;

// err.h contains various nonstandard BSD extensions, but they are        
// very handy.    
#include <err.h>

#include "histogram.h"

int global_histogram[8] = { 0 };

int fhistogram_job(char const *path) {
    FILE *f = fopen(path, "r");

    int local_histogram[8] = { 0 };

    if (f == NULL) {
        assert(pthread_mutex_lock(&stdout_mutex) == 0);
        fflush(stdout);
        warn("failed to open %s", path);
        assert(pthread_mutex_unlock(&stdout_mutex) == 0);
        return -1;
    }

    int i = 0;
    char c;
    const int UPDATE_INTERVAL = 100000;

    while (fread(&c, sizeof(c), 1, f) == 1) {
        i++;
        update_histogram(local_histogram, c);

        if ((i % UPDATE_INTERVAL) == 0) {
            assert(pthread_mutex_lock(&histogram_mutex) == 0);
            merge_histogram(local_histogram, global_histogram);
            assert(pthread_mutex_unlock(&histogram_mutex) == 0);

            
            assert(pthread_mutex_lock(&stdout_mutex) == 0);
            print_histogram(global_histogram);
            assert(pthread_mutex_unlock(&stdout_mutex) == 0);
        }
    }

    fclose(f);

    
    assert(pthread_mutex_lock(&histogram_mutex) == 0);
    merge_histogram(local_histogram, global_histogram);
    assert(pthread_mutex_unlock(&histogram_mutex) == 0);

    
    assert(pthread_mutex_lock(&stdout_mutex) == 0);
    print_histogram(global_histogram);
    assert(pthread_mutex_unlock(&stdout_mutex) == 0);

    return 0;
}


void* worker_histogram(void *arg) {
    struct job_queue *jq = arg;

    while (1) {
        char *path = NULL;
        if (job_queue_pop(jq, (void**)&path) == 0) {
            
            fhistogram_job(path);
            free(path); 
        } else {
            
            break;
        }
    }

    return NULL;
}

int main(int argc, char * const *argv) {
    if (argc < 2) {
        err(1, "usage: paths...");
        exit(1);
    }

    int num_threads = 1;
    char * const *paths = &argv[1];

    if (argc > 3 && strcmp(argv[1], "-n") == 0) {
        num_threads = atoi(argv[2]);

        if (num_threads < 1) {
            err(1, "invalid thread count: %s", argv[2]);
        }

        paths = &argv[3];
    } else {
        paths = &argv[1];
    }

    
    
    
    struct job_queue jq;
    if (job_queue_init(&jq, 64) != 0) {
        err(1, "job_queue_init() failed");
    }

    
    pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
    if (threads == NULL) {
        err(1, "calloc failed for threads");
    }
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, worker_histogram, &jq) != 0) {
            err(1, "pthread_create() failed");
        }
    }

    

    
    int fts_options = FTS_LOGICAL | FTS_NOCHDIR;

    FTS *ftsp;
    if ((ftsp = fts_open(paths, fts_options, NULL)) == NULL) {
        err(1, "fts_open() failed");
        return -1;
    }

    FTSENT *p;
    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
        case FTS_D:
            break;
        case FTS_F:
            if (job_queue_push(&jq, strdup(p->fts_path)) != 0) {
                warn("job_queue_push failed");
            }
            break;
        default:
            break;
        }
    }

    fts_close(ftsp);


    if (job_queue_destroy(&jq) != 0) {
        err(1, "job_queue_destroy() failed");
    }

    
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            err(1, "pthread_join() failed");
        }
    }
    free(threads);

    
    move_lines(9);
    
    

    return 0;
}
