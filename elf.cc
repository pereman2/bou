#include "elf.h"

#include <bits/stdint-intn.h>
#include <elf.h>

#include <cstddef>
#include <cstring>
#include <iostream>

#include "darray.h"
#include "debug.h"
#include "globals.h"

#define debug(x) std::cout << #x << " " << x << std::endl;

#define PUSH_ELF_VAR(da, var)          \
  do {                                 \
    push_dvalue(da, typeof(var), var); \
  } while (0);

// https://refspecs.linuxbase.org/elf/elf.pdf

void file_header(darray* da, std::size_t text_size) {
  push_bytes(da, 4, 0x7f, 0x45, 0x4c, 0x46);                    // elf magic number
  push_bytes(da, 1, 0x02);                                      // 64 bits
  push_bytes(da, 1, 0x01);                                      // Little endian
  push_bytes(da, 1, 0x01);                                      // Current version elf
  push_bytes(da, 1, 0x00);                                      // ABI target / linux
  push_bytes(da, 1, 0x00);                                      // ABI version
  push_bytes(da, 7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  // nothing
  push_bytes(da, 2, 0x02, 0x00);  // object file type, for now just exec
  push_bytes(da, 2, 0x3e, 0x00);  // architecture : amd64-x86
  push_dvalue(da, int32_t, 1);    // eversion
  push_dvalue(da, uint64_t,
              START_ADDR + 0x40 + 0x38);  // execution starts here, section text
  push_dvalue(da, int64_t, 0x40);         // header table offset
  int shoff = 0x40 + 0x38 + text_size + section_name_size;
  debug(section_name_size);
  debug(shoff);
  push_dvalue(da, int64_t, shoff);  // section header table offset
  push_dvalue(da, int32_t, 0x00);   // flags
  push_bytes(da, 2, 0x40, 0x00);    // 64 bytes for 64 bit
  push_dvalue(da, int16_t, 0x38);   // program header size
  push_dvalue(da, int16_t, 1);      // phnum, program header num
  push_dvalue(da, int16_t, 0x40);   // size of section header table entry
  push_dvalue(da, int16_t, 0x03);   // number of section header table entries
  push_dvalue(da, int16_t, 0x02);   // index of section header table entry names
}

void program_header(darray* da, std::size_t text_size) {
  // *** PROGRAM HEADER TABLE ***
  Elf64_Phdr ph;
  int shoff = 0x40 + 0x38 + text_size + section_name_size;
  ph.p_type = PT_LOAD;
  ph.p_flags = PF_X | PF_R;
  ph.p_offset = 0;
  ph.p_vaddr = START_ADDR;
  ph.p_paddr = START_ADDR;
  ph.p_filesz = shoff;
  ph.p_memsz = shoff;
  ph.p_align = 0;

  PUSH_ELF_VAR(da, ph.p_type);
  PUSH_ELF_VAR(da, ph.p_flags);
  PUSH_ELF_VAR(da, ph.p_offset);
  PUSH_ELF_VAR(da, ph.p_vaddr);
  PUSH_ELF_VAR(da, ph.p_paddr);
  PUSH_ELF_VAR(da, ph.p_filesz);
  PUSH_ELF_VAR(da, ph.p_memsz);
  PUSH_ELF_VAR(da, ph.p_align);
}
void add_text(darray* da, darray* text) {
  // *** TEXT ***
  push_darray(da, text);
}

void add_section_names(darray* da, int* section_offsets) {
  // *** SECTION NAMES ***
  push_bytes(da, 1, 0x00);
  for (int i = 0; i < section_names.size(); i++) {
    push_string(da, section_names[i].size() + 1,
                section_names[i].c_str());  // include null terminator
  }
}

void section_header_entry(darray* da, Elf64_Word sh_name, Elf64_Word sh_type, Elf64_Xword sh_flags,
                          Elf64_Addr sh_addr, Elf64_Off sh_offset, Elf64_Xword sh_size,
                          Elf64_Word sh_link, Elf64_Word sh_info, Elf64_Xword sh_addralign,
                          Elf64_Xword sh_entsize) {
  Elf64_Shdr s;
  s.sh_name = sh_name;
  s.sh_type = sh_type;
  s.sh_flags = sh_flags;
  s.sh_addr = sh_addr;
  s.sh_offset = sh_offset;
  s.sh_size = sh_size;
  s.sh_link = sh_link;
  s.sh_info = sh_info;
  s.sh_addralign = sh_addralign;
  s.sh_entsize = sh_entsize;
  PUSH_ELF_VAR(da, s.sh_name);
  PUSH_ELF_VAR(da, s.sh_type);
  PUSH_ELF_VAR(da, s.sh_flags);
  PUSH_ELF_VAR(da, s.sh_addr);
  PUSH_ELF_VAR(da, s.sh_offset);
  PUSH_ELF_VAR(da, s.sh_size);
  PUSH_ELF_VAR(da, s.sh_link);
  PUSH_ELF_VAR(da, s.sh_info);
  PUSH_ELF_VAR(da, s.sh_addralign);
  PUSH_ELF_VAR(da, s.sh_entsize);
}

void section_header_table(darray* da, std::size_t text_size, int* section_offsets) {
  // *** SECTION HEADER TABLE ***
  // null section
  section_header_entry(da, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  // text section
  section_header_entry(da, section_offsets[1], SHT_PROGBITS, (SHF_ALLOC | SHF_EXECINSTR),
                       START_ADDR + 0x40 + 0x38, 0x40 + 0x38, text_size, 0, 0, 1, 0);
  // names section
  section_header_entry(da, section_offsets[0], SHT_STRTAB, 0, 0, 0x40 + 0x38 + text_size,
                       section_name_size, 0, 0, 0, 0);
}

void write_elf(darray* text, std::size_t text_size) {
  darray* da = create_darray();
  init_darray(da);

  int section_offsets[section_names.size()];
  section_name_size = 1;  // include null chars
  for (int i = 0; i < section_names.size(); i++) {
    section_offsets[i] = section_name_size;
    section_name_size += section_names[i].size() + 1;
  }
  DEBUG_ELF_SECTION(file_header(da, text_size), da, "File header");
  DEBUG_ELF_SECTION(program_header(da, text_size), da, "Program header");
  DEBUG_ELF_SECTION(add_text(da, text), da, "Text");
  DEBUG_ELF_SECTION(add_section_names(da, section_offsets), da, "Section names");
  DEBUG_ELF_SECTION(section_header_table(da, text_size, section_offsets), da,
                    "Section header table");

  // *** WRITE TO FILE AND DEBUG ***
  write_to_file(da, "program");
  free_darray(da);
}
