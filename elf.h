#pragma once

#include "darray.h"


#define START_ADDR 0x400000
#define DATA_START_ADDR 0x600000
#define ALIGNMENT 0x200000

void write_elf(darray *da);
