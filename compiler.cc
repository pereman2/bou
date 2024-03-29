#include "compiler.h"

#include <stdlib.h>

#include <cstdarg>
#include <cstring>

#include "darray.h"
#include "debug.h"
#include "elf.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"

compiler_state state;
// this array contains the registers ordered by code:
// 000 eax 001 ecx 010 edx 011 ebx 100 esp
// 101 ebp 110 esi 111 edi
const char* ordered_registers[NUM_REGISTERS] = {"eax", "ecx", "edx", "ebx",
                                                "esp", "ebp", "esi", "edi"};

void compile(char* src) {
  Token** tokens = scan(src);
  int len_tokens = get_tokens_len();
  print_tokens(tokens, len_tokens);
  Ast_node* root = parse(tokens, len_tokens);

  exit(1);
  init_compiler();
  compile_ast(root);

  // sys exit
  push_bytes_code(5, 0xb8, 0x01, 0x00, 0x00, 0x00);
  push_bytes_code(5, 0xbb, 0x00, 0x00, 0x00, 0x00);  // mov rbx, 0x0
  push_bytes_code(2, 0xcd, 0x80);                    // int 0x80

  write_elf(&state.code, state.code.count);
  free_compiler();
  exit(1);
}

void init_compiler() {
  state.register_descriptors = (int*)malloc(sizeof(int) * NUM_REGISTERS);
  for (int rd = 0; rd < NUM_REGISTERS; rd++) {
    state.register_descriptors[rd] = 0;
  }
  state.register_ip = 0;
  init_darray(&state.code);
}

void free_compiler() { free_darray(&state.code); }

char next_register() {
  if (memcmp(ordered_registers[state.register_ip], "esp", 3) == 0) {
    state.register_ip += 2;  // skip esp and ebp for now
  }
  if (state.register_ip + 1 == NUM_REGISTERS) {
    state.register_ip = 0;
    return 0;
  }
  return state.register_ip++;
}

void compile_ast(Ast_node* node) {
  switch (node->type) {
      // TODO: literals logic should be nil for now.
    case node_type::LITERAL: {
      // char r = next_register();
      // operand dest = {type: operand::REG, size: 32};
      // dest.value.reg = r;
      // operand src = {type: operand::IMM, size: 32};
      // src.value.imm = get_literal(node)->value.i;
      // emit_mov(dest, src);
    } break;
    case node_type::BINARY:
      compile_binary_expr(node);
      break;
  }
}

void compile_binary_expr(Ast_node* node) {
  if (get_binary(node).left->type == node_type::BINARY) {
    compile_ast(get_binary(node).left);
  } else if (get_binary_left(node)->type == node_type::LITERAL) {
    if (state.register_descriptors[0]) {
      operand dest = {type : operand::REG, size : 32};
      dest.value.reg = 0;
      operand src = {type : operand::REG, size : 32};
      src.value.reg = 1;
      emit_mov(dest, src);
    } else {
      operand dest = {type : operand::REG, size : 32};
      dest.value.reg = 0;
      operand src = {type : operand::IMM, size : 32};
      src.value.imm = get_literal(get_binary_left(node)).value;
      emit_mov(dest, src);
    }
  }
  if (get_binary(node).right->type == node_type::BINARY) {
    compile_ast(get_binary(node).right);
  } else if (get_binary(node).right->type == node_type::LITERAL) {
    operand dest = {type : operand::REG, size : 32};
    dest.value.reg = 1;
    operand src = {type : operand::IMM, size : 32};
    src.value.imm = get_literal(get_binary_right(node)).value;
    emit_mov(dest, src);
  }
  switch ((get_binary(node)).op) {
    case binary_type::ADD: {
      // for now let's expect only register in add
      operand dest = {type : operand::REG, size : 32};
      dest.value.reg = 0;
      operand src = {type : operand::REG, size : 32};
      src.value.reg = 1;
      emit_add(dest, src);
    } break;
    case binary_type::SUB:
      break;
  }
  state.register_descriptors[0] = 1;
}

void emit_add(operand dest, operand src) {
  if (dest.type == operand::REG && src.type == operand::REG) {  // 32 reg add reg, reg
    // modR/M -> 2 bits mode, 3 bits reg/m, 3 bits register
    char mod_r_w = (0x03 << 6) | (src.value.reg << 3) | dest.value.reg;
    push_bytes_code(2, 0x01, mod_r_w);
  }
}

void emit_mov(operand dest, operand src) {
  if (dest.type == operand::REG && src.type == operand::IMM) {  // 32 imm mov reg, imm
    // b8+ rd id = b8 + register id
    push_bytes(&state.code, 5, 0xb8 + dest.value.reg, src.value.imm, 0x00, 0x00, 0x00);
  } else if (dest.type == operand::REG && src.type == operand::REG) {
    char mod_r_w = (0x03 << 6) | (src.value.reg << 3) | dest.value.reg;
    // push_bytes(&state.code, 5, 0xb8, mod_r_w, 0x00, 0x00, 0x00);
  }
}
