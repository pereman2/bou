#include "darray.h"

#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>

#include <cassert>
#include <cstdarg>
#include <cstddef>

#include "util.h"
#include "arena.h"


darray* create_darray() {
  Allocator *dumb = allocator_create(DUMB, 0);
  darray* da = (darray*)malloc(sizeof(darray));
  da->allocator = dumb;
  return da;
}

darray* create_darray(Allocator *allocator) {
  darray* da = (darray*)malloc(sizeof(darray));
  da->allocator = allocator;
  return da;
}

void init_darray(darray* da) {
  da->capacity = 8;
  da->count = 0;
  da->allocator = DUMB_ALLOCATOR;
  da->src = (char*)ALLOCATE(da->allocator, 8);
}

void init_darray(darray* da, Allocator *allocator) {
  assert(allocator);
  da->capacity = 8;
  da->count = 0;
  da->allocator = allocator;
  da->src = (char*)ALLOCATE(da->allocator, 8);
}

void free_darray(darray* da) { free(da->src); }

// Push same bytes a number of times
void push_replicated_byte(darray* da, int num_bytes, int byte) {
  darray_grow(da, num_bytes);
  memset(&da->src[da->count], byte, num_bytes);
}

/*
 * Extra arguments can ben any type with same or less rank as int (I think lmao)
 */
void push_bytes(darray* da, int num_bytes, ...) {
  va_list valist;
  va_start(valist, num_bytes);

  darray_grow(da, num_bytes);
  for (int i = 0; i < num_bytes; i++) {
    da->src[da->count++] = va_arg(valist, int);
  }
}

void push_darray(darray* dest, darray* src) {
  for (int i = 0; i < src->count; i++) {
    push_dvalue(dest, char, src->src[i]);
  }
}

void push_string(darray* da, int num_bytes, const char* str) {
  darray_grow(da, num_bytes);
  memcpy(&da->src[da->count], str, num_bytes);
  da->count += num_bytes;
}

void grow_darray(darray* da) {
  std::size_t new_capacity = (da->capacity << 1);
  char* r = (char*)ALLOCATE(da->allocator, new_capacity);
  memcpy(r, da->src, da->capacity);
  DEALLOCATE(da->allocator, da->src);
  ensure_alloc(r, __FILE__, __LINE__);
  da->capacity = new_capacity;
  da->src = r;
}

void write_to_file(darray* da, const char* path) {
  FILE* fd = fopen(path, "w+");
  if (fd == NULL) {
    printf("Error opening file to write darray in %s", path);
    exit(1);
  }

  fwrite(da->src, sizeof(char), da->count, fd);
  fclose(fd);
}

std::size_t length(darray* da) { return da->count; }

void* darray_get(darray* da, size_t size, int i) { return (void*)(da->src + (size * i)); }

void darray_push(darray* da, size_t size, void* value) {
  darray_grow(da, size);
  memcpy(&da->src[da->count], value, size);
  da->count += size;
}

size_t darray_length(darray* da, size_t size) {
  assert(size > 0);
  return da->count / size;
}

void darray_reserve(darray* da, size_t size, size_t number) {
  size_t bytes = size * number;
  darray_grow(da, bytes);
  da->count += bytes;
}

int darray_remove(darray* da, size_t size, int i) {
  if (i >= darray_length(da, size)) {
    return ENOENT;
  }
  size_t left = darray_length(da, size) - i - 1;
  if (left) {
    memcpy(da->src + (size * i), da->src + (size * (i + 1)), left * size);
  }
  da->count -= size;
  return 0;
}
