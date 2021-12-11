#include <cstdio>


#define ensure_alloc(ptr, file, line)                                   \
  {                                                                     \
    if (ptr == NULL) {                                                  \
      printf("Error allocating/reallocating at %s:%d \n", file, line);  \
      exit(1);                                                          \
    }                                                                   \
  }
