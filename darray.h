#pragma once

#include <cstddef>

#define push_dvalue(da, type, value)  {               \
    while (sizeof(type) + da->count > da->capacity) { \
      grow_darray(da);                                \
    }                                                 \
    type *i_pos = (type*)&da->src[da->count];         \
    *i_pos = value;                                   \
    da->count += sizeof(type);                        \
  }                                                   \

typedef struct darray {
  char *src;
  std::size_t capacity, count;
} darray;

void push_replicated_byte(darray *da, int num, int byte);
void push_bytes(darray *da, int num, ...);
void grow_darray(darray *da);
void init_darray(darray *da);
void free_darray(darray *da);
void write_to_file(darray *da, const char *path);
