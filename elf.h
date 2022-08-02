#pragma once

#include "darray.h"
#include <string>
#include <vector>


#define START_ADDR 0x80000000

static const std::vector<std::string> section_names = {
  ".shrtrtab",
  ".text"
};
static std::size_t section_name_size; 

void write_elf(darray *text, size_t text_size);
