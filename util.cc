#include "util.h"

#include <cstdint>

#include "darray.h"

void string_init(bou_string* str) {
  init_darray(&str->data);
}

void string_init_from(bou_string* str, char* raw_str, uint64_t len) { 
  string_init(str);
  push_string(&str->data, len, raw_str);
  push_bytes(&str->data, 1, '\0');
}

char* string_ref(bou_string* str) {
  return str->data.src;
}
