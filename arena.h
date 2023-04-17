#pragma once

#include <cstdint>
#include <cstdlib>


#define ALLOCATE(allocator, amount) allocator->allocate(allocator->data, amount)
#define DEALLOCATE(allocator, ptr) allocator->deallocate(allocator->data, ptr)

#define DUMB_ALLOCATOR allocator_create(DUMB, 0);

typedef void* (*allocate_func)(void*, size_t);
typedef void (*deallocate_func)(void*, void*);

typedef struct Allocator {
    allocate_func allocate;
    deallocate_func deallocate;
    void *data;
} Allocator;

struct Arena {
  uint8_t* bytes;
  uint64_t size;
  uint64_t offset;
};

Allocator* arena_create_and_init(uint64_t size_of_arena);
void* arena_allocate(void* arena, uint64_t amount);
void arena_deallocate(void* arena, void* pos);
void arena_free(Arena* arena);

Allocator* dumb_create_and_init(uint64_t size);
void* dumb_allocate(void *data, uint64_t amount);
void dumb_deallocate(void *data, void* ptr);

enum AllocatorType {
  DUMB,
  ARENA
};

Allocator* allocator_create(AllocatorType type, uint64_t default_size);
