#include "globals.h"
#include "elf.h"
#include "darray.h"

#include <elf.h>

// https://refspecs.linuxbase.org/elf/elf.pdf

void write_elf(darray *da, size_t text_size) {
  // File header
  push_bytes(da, 4, 0x7f, 0x45, 0x4c, 0x46); // elf magic number
  push_bytes(da, 1, 0x02); // 64 bits
  push_bytes(da, 1, 0x01); // Little endian
  push_bytes(da, 1, 0x01); // Current version elf
  push_bytes(da, 1, 0x00); // ABI target / linux
  push_bytes(da, 1, 0x00); // ABI version
  push_bytes(da, 7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // nothing
  push_bytes(da, 2, 0x02, 0x00); // object file type, for now just exec
  push_bytes(da, 2, 0x3e, 0x00); // architecture : amd64
  // push_bytes(da, 4, 0x01, 0x00, 0x00, 0x00); // elf version, 1 == current
  // let's use 64 bits every time, I just don't want to deal with other things rn :P
  push_dvalue(da, int32_t, 1); // eversion
  uint64_t text_offset = 0x40 + (1 * 0x38);
  push_dvalue(da, uint64_t, START_ADDR + text_offset);
  push_dvalue(da, int64_t, 0x40); // header table offset
  push_dvalue(da, int64_t, 0x00); // start section header table
  push_dvalue(da, int32_t, 0x00); // flags
  push_bytes(da, 2, 0x40, 0x00); // 64 bytes for 64 bit
  push_bytes(da, 2, 0x38, 0x00); // program header size
  push_bytes(da, 2, 0x01, 0x00); // number of entries in program header table: data + text
  push_bytes(da, 2, 0x00, 0x00); // size of section header table entry
  push_bytes(da, 2, 0x00, 0x00); // number of section header table entries
  push_bytes(da, 2, 0x00, 0x00); // index of section header table entry names


  // text section

  // FIXME: change size to represent the true size of the text+data segments
  push_dvalue(da, int32_t, 1); // type of segment/load
  push_dvalue(da, int32_t, 0x05); // flags xwr
  push_dvalue(da, int64_t, text_offset); // text offset
  push_dvalue(da, int64_t, START_ADDR + text_offset); // vstart
  push_dvalue(da, int64_t, 0); // pstart
  push_dvalue(da, int64_t, text_size); // segment size in bytes in file image
  push_dvalue(da, int64_t, text_size); // segment size in bytes in memory
  push_dvalue(da, int64_t, ALIGNMENT); // alignment
  // // data section

  // int data_offset = text_offset + text_size;

  // push_dvalue(da, int32_t, 1); // type of segment/load
  // push_dvalue(da, int32_t, 0x07); // flags xwr
  // push_dvalue(da, int64_t, data_offset); // text offset
  // push_dvalue(da, int64_t, DATA_START_ADDR); // vstart
  // push_dvalue(da, int64_t, DATA_START_ADDR); // pstart
  // push_dvalue(da, int64_t, 0); // segment size in bytes in file image
  // push_dvalue(da, int64_t, 0); // segment size in bytes in memory
  // push_dvalue(da, int64_t, ALIGNMENT); // alignment
}
