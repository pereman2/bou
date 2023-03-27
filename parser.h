#ifndef PARSER
#define PARSER

#include <map>
#include <string>

#include "arena.h"
#include "ast_node.h"
#include "token.h"

struct Parser {
  Token** tokens;
  int ntokens;
  int ip;
  Arena* arena;
  std::map<std::string, AstIdentifier> identifiers;
};

Ast_node* parse(Token** tokens, int ntoken);
Ast_node* create_ast_node(node_type type);

Ast_node* statement();
Ast_node* parse_block();
Ast_node* expression(bool semicolon = true);

Ast_node* decl();
Ast_node* ident();
Ast_node* logic();
Ast_node* comparison();
Ast_node* assignment();

Ast_node* term();
Ast_node* factor();
Ast_node* unary();
Ast_node* deref();
Ast_node* ref();
Ast_node* literal();

bool is_type_id(int pos = 0);
void ast_do_binary_op_save_left(AstLiteral* left, AstLiteral* right);

#endif  // PARSER
