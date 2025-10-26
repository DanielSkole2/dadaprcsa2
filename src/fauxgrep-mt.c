#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <err.h>

#include "job_queue.h"


pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

int global_matches_found = 0;
pthread_mutex_t matches_mutex = PTHREAD_MUTEX_INITIALIZER;


struct worker_args {
    struct job_queue *jq;
    char const *needle;
};



int fauxgrep_file(char const *needle, char const *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        assert(pthread_mutex_lock(&stdout_mutex) == 0);
        warn("failed to open %s", path);
        assert(pthread_mutex_unlock(&stdout_mutex) == 0);
        
        return 2; 
    }

    char *line = NULL;
    size_t len = 0;
    int matches_in_file = 0;
    int line_num = 0;

    while (getline(&line, &len, f) != -1) {
        line_num++;
        if (strstr(line, needle) != NULL) {
            matches_in_file = 1;
            
            
            assert(pthread_mutex_lock(&stdout_mutex) == 0);
            printf("%s:%d: %s", path, line_num, line);
            
            fflush(stdout); 
            assert(pthread_mutex_unlock(&stdout_mutex) == 0);

            
            assert(pthread_mutex_lock(&matches_mutex) == 0);
            global_matches_found = 1;
            assert(pthread_mutex_unlock(&matches_mutex) == 0);
        }
    }
    
    free(line);
    fclose(f);
    
    
    return matches_in_file ? 0 : 1; 
}



void* worker(void *arg) {
    struct worker_args *args = arg;
    struct job_queue *jq = args->jq;
    const char *needle = args->needle;

    while (1) {
        char *path = NULL;
        int status = job_queue_pop(jq, (void**)&path);
        
        if (status == 0) {
            
            fauxgrep_file(needle, path); 
            free(path); 
        } else if (status == -1) {
            
            break;
        } else {
            
            break; 
        }
    }

    return NULL;
}


int main(int argc, char * const *argv) {
    if (argc < 2) {
        errx(1, "usage: [-n INT] STRING paths...");
        exit(1);
    }

    int num_threads = 1;
    char const *needle = NULL;
    char * const *paths = NULL;
    int current_arg = 1;


    
    if (argc >= 4 && strcmp(argv[1], "-n") == 0) {
        num_threads = atoi(argv[2]);

        if (num_threads < 1) {
            errx(1, "invalid thread count: %s", argv[2]);
        }

        current_arg = 3;
    }

    
    if (argc - current_arg < 2) {
        errx(1, "usage: [-n INT] STRING paths...");
    }

    needle = argv[current_arg];
    paths = &argv[current_arg + 1];

    
    struct job_queue *jq = malloc(sizeof(struct job_queue));
    if (jq == NULL) {
        err(1, "malloc failed for job_queue");
    }
    if (job_queue_init(jq, 64) != 0) {
        free(jq);
        err(1, "job_queue_init() failed");
    }

    
    struct worker_args args = { .jq = jq, .needle = needle };

    
    pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
    if (threads == NULL) {
        free(jq);
        err(1, "calloc failed for threads");
    }

    for (int i = 0; i < num_threads; i++) {
        
        if (pthread_create(&threads[i], NULL, worker, &args) != 0) {
            free(jq);
            free(threads);
            err(1, "pthread_create() failed");
        }
    }

    
    
    int fts_options = FTS_LOGICAL | FTS_NOCHDIR;
    FTS *ftsp;
    if ((ftsp = fts_open(paths, fts_options, NULL)) == NULL) {
        err(1, "fts_open() failed");
        return 2; 
    }

    FTSENT *p;
    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
        case FTS_D:
            break;
        case FTS_F:
            if (job_queue_push(jq, strdup(p->fts_path)) != 0) {
                warn("job_queue_push failed");
            }
            break;
        default:
            break;
        }
    }
    fts_close(ftsp);


    
    if (job_queue_destroy(jq) != 0) {
        err(1, "job_queue_destroy() failed");
    }

    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    
    assert(pthread_mutex_destroy(&jq->lock) == 0);
    assert(pthread_cond_destroy(&jq->not_empty) == 0);
    assert(pthread_cond_destroy(&jq->not_full) == 0);
    free(jq->buffer);

    free(threads);
    free(jq);

    
    return global_matches_found ? 0 : 1; 
}