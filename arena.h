#pragma once

#include <cstdint>
#include <cstdlib>

struct Arena {
  uint8_t* bytes;
  uint64_t size;
  uint64_t offset;
};

Arena* arena_create_and_init(uint64_t size_of_arena); 
void* arena_allocate(Arena *arena, uint64_t amount); 
void arena_deallocate(Arena *arena, void* pos); 
void arena_free(Arena *arena); 
