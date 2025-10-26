#ifndef HISTOGRAM_H
#define HISTOGRAM_H


static void move_lines(int n) {
  if (n < 0) {
    printf("\033[%dA", -n);
  } else {
    printf("\033[%dB", n);
  }
}


static void clear_line() {
  printf("\033[K");
}


static void print_histogram(int histogram[8]) {
  int64_t bits_seen = 0;

  for (int i = 0; i < 8; i++) {
    bits_seen += histogram[i];
  }

  for (int i = 0; i < 8; i++) {
    clear_line();
    printf("Bit %d: ", i);

    double proportion = histogram[i] / ((double)bits_seen);
    for (int i = 0; i < 60*proportion; i++) {
      printf("*");
    }
    printf("\n");
  }

  clear_line();
  printf("%ld bits processed.\n", (long)bits_seen);
  move_lines(-9);
}

static void merge_histogram(int from[8], int to[8]) {
  for (int i = 0; i < 8; i++) {
    to[i] += from[i];
    from[i] = 0;
  }
}

static void update_histogram(int histogram[8], unsigned char byte) {
  
  for (int i = 0; i < 8; i++) {
    
    if (byte & (1<<i)) {
      histogram[i]++;
    }
  }
}

#endif
