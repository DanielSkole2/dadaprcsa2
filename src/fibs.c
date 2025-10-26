#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>


#include <err.h>

#include "job_queue.h"


pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;


int fib (int n) {
  if (n < 2) {
    return 1;
  } else {
    return fib(n-1) + fib(n-2);
  }
}


void fib_line(const char *line) {
  int n = atoi(line);
  int fibn = fib(n);
  assert(pthread_mutex_lock(&stdout_mutex) == 0);
  printf("fib(%d) = %d\n", n, fibn);
  assert(pthread_mutex_unlock(&stdout_mutex) == 0);
}


void* worker(void *arg) {
  struct job_queue *jq = arg;

  while (1) {
    char *line;
    if (job_queue_pop(jq, (void**)&line) == 0) {
      fib_line(line);
      free(line);
    } else {
      
      break;
    }
  }

  return NULL;
}

int main(int argc, char * const *argv) {
  int num_threads = 1;

  if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    num_threads = atoi(argv[2]);

    if (num_threads < 1) {
      err(1, "invalid thread count: %s", argv[2]);
    }
  }

  
  struct job_queue jq;
  job_queue_init(&jq, 64);

  
  pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
  for (int i = 0; i < num_threads; i++) {
    if (pthread_create(&threads[i], NULL, &worker, &jq) != 0) {
      err(1, "pthread_create() failed");
    }
  }


  
  char *line = NULL;
  ssize_t line_len;
  size_t buf_len = 0;
  while ((line_len = getline(&line, &buf_len, stdin)) != -1) {
    job_queue_push(&jq, (void*)strdup(line));
  }
  free(line);

  
  job_queue_destroy(&jq);

  
  for (int i = 0; i < num_threads; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      err(1, "pthread_join() failed");
    }
  }
  free(threads);
}
