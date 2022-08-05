/*
 * This compiler is meant to target x86_64 intel only. :)
*/
#pragma once

#include "darray.h"
#include "ast_node.h"

/*
 * Push bytes to the code.state darray.
*/
#define push_bytes_code(len, ...) push_bytes(&state.code, len, ##__VA_ARGS__)
typedef struct operand operand;

typedef enum instruction_type {
  I_MOV,
  I_ADD,
  I_SUB
} instruction_type;

struct operand {
  enum {
    IMM,
    REG,
    ADDR
  } type;

  union {
    char reg; // 3 bit representation of the register id intel
    void *address;
    int imm;
  } value;

  int size; // 8, 16, 32, 64 bits...
  // https://www.cs.virginia.edu/~evans/cs216/guides/x86.html
};

#define NUM_REGISTERS 8

/*
- instruction
  prefix opcode modR/M SIB displacement inmediate

- registers
*/
   /* 0:   b9 01 00 00 00          mov    $0x1,%ecx */
   /* 5:   89 c1                   mov    %eax,%ecx */
   /* 7:   48 b8 3c 00 00 00 00    movabs $0x3c,%rax */

typedef struct compiler_state {
  char register_ip;
  darray code;
  int *register_descriptors;
} compiler_state;

extern compiler_state state;

void compile(char* src);
void init_compiler();
void free_compiler();
void compile_ast(Ast_node *node);
void compile_binary_expr(Ast_node *node);
char next_register();
void emit_mov(operand dest, operand src);
void emit_add(operand dest, operand src);
void emit(instruction_type type, operand dest, operand src);
