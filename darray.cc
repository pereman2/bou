#include <cstddef>
#include <cstdarg>
#include <stdlib.h>
#include <string.h>
#include "darray.h"
#include "util.h"


void init_darray(darray *da) {
  da->capacity = 8;
  da->count = 0;
  da->src = (char*) malloc(8);
}

void free_darray(darray *da) {
  free(da->src);
}

// Push same bytes a number of times
void push_repicated_byte(darray* da, int num_bytes, int byte) {
  while (num_bytes + da->count > da->capacity) {
    grow_darray(da);
  }
  memset(&da->src[da->count], byte, num_bytes);
}

/*
 * Extra arguments can ben any type with same or less rank as int (I think lmao)
*/
void push_bytes(darray* da, int num_bytes, ...) {
  va_list valist;
  va_start(valist, num_bytes);

  while (num_bytes + da->count > da->capacity) {
    grow_darray(da);
  }
  for (int i = 0; i < num_bytes; i++) {
    da->src[da->count++] = va_arg(valist, int);
  }
}

void grow_darray(darray *da) {
  std::size_t new_capacity = (da->capacity << 1);
  char *r = (char*)realloc(da->src, new_capacity);
  ensure_alloc(r, __FILE__, __LINE__);
  da->capacity = new_capacity;
  da->src = r;
}


void write_to_file(darray *da, const char* path) {
  FILE *fd = fopen(path, "w+");
  if(fd == NULL) {
    printf("Error opening file to write darray in %s", path);
    exit(1);
  }

  fwrite(da->src, sizeof(char), da->count, fd);
  fclose(fd);
}
