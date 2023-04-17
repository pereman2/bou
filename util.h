#pragma once

#include <cstdint>
#include <cstdio>
#include "darray.h"

#define KB(a) a * 1024
#define MB(a) a * 1048576

#define ensure_alloc(ptr, file, line)                                  \
  {                                                                    \
    if (ptr == NULL) {                                                 \
      printf("Error allocating/reallocating at %s:%d \n", file, line); \
      exit(1);                                                         \
    }                                                                  \
  }


struct bou_string {
  darray data;
};

void string_init(bou_string* str);
void string_init_from(bou_string* str, char* raw_str, uint64_t len);
char* string_ref(bou_string* str);

