#pragma once

#include <string>

#include "ast_node.h"
#include "darray.h"
#include "token.h"

#define DEBUG_ELF_SECTION(func, da, name)                                                         \
  do {                                                                                            \
    int _s1 = length(da);                                                                         \
    std::cout << "Start of section " << name << " is: 0x" << std::hex << length(da) << std::endl; \
    func;                                                                                         \
    std::cout << "Size of section " << name << " is: 0x" << std::hex << length(da) - _s1          \
              << std::endl;                                                                       \
  } while (0);

void print_token(Token* t);
void print_ast(Ast_node* root);
void eval_ast(Ast_node* root);
std::string ast_node_to_string(Ast_node* root);
std::string ast_statement_to_string(Ast_node* root);
std::string ast_expression_to_string(Ast_node* root);
std::string ast_block_to_str(AstBlock* block, std::string block_name);
void print_tokens(Token** tokens, int len);
std::string literal_repr(AstLiteral* literal);
void dump_darray(darray* da);
void dump_darray(darray* da);
