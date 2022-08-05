#pragma once

#include "darray.h"
#include "ast_node.h"
#include "token.h"
#include <string>

#define DEBUG_ELF_SECTION(func, da, name)                   \
  do {                                                      \
    int _s1 = length(da);                                   \
    std::cout << "Start of section " << name << " is: 0x"   \
    << std::hex << length(da) << std::endl;       \
    func;                                                   \
    std::cout << "Size of section " << name << " is: 0x"      \
    << std::hex << length(da) - _s1 << std::endl; \
  }                                                         \
  while (0);                                                \

void print_token(Token *t);
void print_ast(Ast_node* root);
void eval_ast(Ast_node* root);
std::string ast_node_to_string(Ast_node* root);
void print_tokens(Token **tokens, int len);
void dump_darray(darray *da);
void dump_darray(darray *da);
