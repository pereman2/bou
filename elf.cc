#include "globals.h"
#include "elf.h"
#include "darray.h"



void write_elf(darray *da) {
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
  push_dvalue(da, int64_t, START_ADDR);
  push_dvalue(da, int64_t, 0x40); // header table offset
  push_dvalue(da, int64_t, 0x00); // start section header table
  push_dvalue(da, int32_t, 0x00); // flags
  push_bytes(da, 2, 0x40, 0x00); // 64 bytes for 64 bit
  push_bytes(da, 2, 0x38, 0x00); // program header size
  push_bytes(da, 2, 0x02, 0x00); // number of entries in program header table: data + text
  push_bytes(da, 2, 0x40, 0x00); // size of section header table entry
  push_bytes(da, 2, 0x00, 0x00); // number of section header table entries
  push_bytes(da, 2, 0x00, 0x00); // index of section header table entry names


  // text section

  // FIXME: change size to represent the true size of the text+data segments
  push_dvalue(da, int32_t, 1); // type of segment/load
  push_dvalue(da, int32_t, 7); // flags xwr
  push_dvalue(da, int64_t, 0); // text offset
  push_dvalue(da, int64_t, START_ADDR); // vstart
  push_dvalue(da, int64_t, START_ADDR); // pstart
  push_dvalue(da, int64_t, 4096); // segment size in bytes in file image
  push_dvalue(da, int64_t, 4096); // segment size in bytes in memory
  push_dvalue(da, int32_t, 0); // segment flags
  push_dvalue(da, int64_t, ALIGNMENT); // alignment
  // data section

  push_dvalue(da, int32_t, 1); // type of segment/load
  push_dvalue(da, int32_t, 7); // flags xwr
  push_dvalue(da, int64_t, 0); // text offset
  push_dvalue(da, int64_t, DATA_START_ADDR); // vstart
  push_dvalue(da, int64_t, DATA_START_ADDR); // pstart
  push_dvalue(da, int64_t, 4096); // segment size in bytes in file image
  push_dvalue(da, int64_t, 4096); // segment size in bytes in memory
  push_dvalue(da, int32_t, 0); // segment flags
  push_dvalue(da, int64_t, ALIGNMENT); // alignment
}
