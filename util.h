#include <cstdio>

#define KB(a) a * 1024
#define MB(a) a * 1048576

#define ensure_alloc(ptr, file, line)                                  \
  {                                                                    \
    if (ptr == NULL) {                                                 \
      printf("Error allocating/reallocating at %s:%d \n", file, line); \
      exit(1);                                                         \
    }                                                                  \
  }
