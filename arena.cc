#include "arena.h"

Arena* arena_create_and_init(uint64_t size_of_arena) {
  Arena* arena = (Arena*) malloc(sizeof(Arena));
  arena->size = size_of_arena;
  arena->offset = 0;
  arena->bytes = (uint8_t*) malloc(size_of_arena);
  return arena;
}

void* arena_allocate(Arena *arena, uint64_t amount) {
  if (arena->offset + amount > arena->size) {
    return NULL;
  }

  // TODO: add alignment, but first we need to align our priorities
  void* pos = arena->bytes + arena->offset;
  arena->offset += amount;
  return pos;
}


void arena_deallocate(Arena *arena, void* pos) {
  // memory is free babyy
}


void arena_free(Arena *arena) {
  free(arena->bytes);
  free(arena);
}
