#include <cstdarg>
#include <cstring>
#include <stdlib.h>
#include "compiler.h"
#include "darray.h"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "elf.h"

compiler_state state;
// this array contains the registers ordered by code:
// 000 eax 001 ecx 010 edx 011 ebx 100 esp
// 101 ebp 110 esi 111 edi
const char *ordered_registers[NUM_REGISTERS] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};

void compile(char *src) {
  Token **tokens = scan(src);
  int len_tokens = get_tokens_len();
  print_tokens(tokens, len_tokens);
  Ast_node *root = parse(tokens, len_tokens);

  init_compiler();
  write_elf(&state.code);
  compile_ast(root);
  dump_darray(&state.code);
  write_to_file(&state.code, "program");
  free_compiler();
  exit(1);
}

void init_compiler() {
  state.register_ip = 0;
  init_darray(&state.code);
}

void free_compiler() {
  free_darray(&state.code);
}

char next_register() {
  if (memcmp(ordered_registers[state.register_ip], "esp", 3) == 0) {
    state.register_ip += 2; // skip esp and ebp for now
  }
  if (state.register_ip + 1 == NUM_REGISTERS) {
    state.register_ip = 0;
    return 0;
  }
  return state.register_ip++;
}

void compile_ast(Ast_node *node) {
  switch (node->type) {
  case node_type::LITERAL:
    {
      char r = next_register();
      operand dest = {type: operand::REG, size: 32};
      dest.value.reg = r;
      operand src = {type: operand::IMM, size: 32};
      src.value.imm = get_literal(node)->value.i;
      emit_mov(dest, src);
    }
    break;
  case node_type::BINARY:
    compile_binary_expr(node);
    break;
  }
}

void compile_binary_expr(Ast_node *node) {
  compile_ast(get_binary(node)->left);
  compile_ast(get_binary(node)->right);
  switch((get_binary(node))->op) {
  case Ast_binary::ADD:
    {
      // for now let's expect only register in add
      operand dest = {type: operand::REG, size: 32};
      dest.value.reg = state.register_ip - 2;
      operand src = {type: operand::REG, size: 32};
      src.value.reg = state.register_ip - 1;
      emit_add(dest, src);
    }
    break;
  case Ast_binary::SUB:
    break;
  }
}

void emit_add(operand dest, operand src) {
  if (dest.type == operand::REG && src.type == operand::REG) { // 32 reg add reg, reg
    char mod_r_w = (src.value.reg << 3) | dest.value.reg | 0x02;
    push_bytes_code(2, 0x01, mod_r_w);
  }
}
void emit_mov(operand dest, operand src) {
  if (dest.type == operand::REG && src.type == operand::IMM) { // 32 imm mov reg, imm
    // b8+ rd id = b8 + register id
    push_bytes(&state.code, 2, 0xb8+dest.value.reg, src.value.imm);
  }
}

// void emit(instruction_type type, operand dest, operand src) {
//   // put_bytes(state.code, 2);
// }
