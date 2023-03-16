#pragma once

#include <cstddef>
#include <cstring>

#define darray_grow(da, amount)                                                \
  {                                                                            \
    while (amount + da->count > da->capacity) {                                \
      grow_darray(da);                                                         \
    }                                                                          \
  }

#define push_dvalue(da, type, value)                                           \
  {                                                                            \
    darray_grow(da, sizeof(type)) type *i_pos = (type *)&da->src[da->count];   \
    *i_pos = value;                                                            \
    da->count += sizeof(type);                                                 \
  }


typedef struct darray {
  char *src;
  std::size_t capacity, count;
} darray;

darray *create_darray();

void *darray_get(darray *da, size_t size, int i);
void darray_push(darray *da, size_t size, void* value);
size_t darray_length(darray *da, size_t size);
void darray_reserve(darray *da, size_t size, size_t number);

/* raw functions */
void push_replicated_byte(darray *da, int num, int byte);
void push_bytes(darray *da, int num, ...);
void push_darray(darray *dest, darray *src);
void push_string(darray *da, int num_bytes, const char *str);
void grow_darray(darray *da);
void init_darray(darray *da);
void free_darray(darray *da);
std::size_t length(darray *da);
void write_to_file(darray *da, const char *path);
