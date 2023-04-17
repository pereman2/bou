#include "arena.h"

#include <cstdio>
#include <cstdlib>


Allocator* arena_create_and_init(uint64_t size_of_arena) {
  Arena* arena = (Arena*)malloc(sizeof(Arena));
  arena->size = size_of_arena;
  arena->offset = 0;
  arena->bytes = (uint8_t*)malloc(size_of_arena);

  Allocator* arena_allocator = (Allocator*)malloc(sizeof(Allocator));
  arena_allocator->allocate = arena_allocate;
  arena_allocator->deallocate = arena_deallocate;
  arena_allocator->data = (void*)arena;
  return arena_allocator;
}

void* arena_allocate(void* arena_ptr, uint64_t amount) {
  Arena* arena = (Arena*)arena_ptr;
  if (arena->offset + amount > arena->size) {
    printf("ERROR: arena allocation failed");
    abort();
    return NULL;
  }

  // TODO: add alignment, but first we need to align our priorities
  void* pos = arena->bytes + arena->offset;
  arena->offset += amount;
  return pos;
}

void arena_deallocate(void* arena, void* pos) {
  // memory is free babyy
}

void arena_free(Arena* arena) {
  // not that free
  free(arena->bytes);
  free(arena);
}

void* dumb_allocate(void *data, uint64_t amount) {
  return malloc(amount);
}

void dumb_deallocate(void *data, void* ptr) {
  free(ptr);
}

Allocator* dumb_create_and_init(uint64_t size) {
  Allocator *allocator = (Allocator*)malloc(sizeof(Allocator));
  allocator->allocate = dumb_allocate;
  allocator->deallocate = dumb_deallocate;
  return allocator;
}

Allocator* allocator_create(AllocatorType type, uint64_t default_size) {
  switch(type) {
    case ARENA:
      return arena_create_and_init(default_size);
    case DUMB:
      return dumb_create_and_init(default_size);
  }
      return dumb_create_and_init(default_size);
}
