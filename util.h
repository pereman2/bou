

#define ensure_alloc(ptr, line, file)                                          \
  {                                                                            \
    if (ptr == NULL) {                                                         \
      printf("Error allocating/reallocating at %s:%d \n", file, line);         \
      exit(1);                                                                 \
    }                                                                          \
  }
