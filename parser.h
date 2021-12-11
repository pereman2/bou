#ifndef PARSER
#define PARSER

#include "token.h"
#include "ast_node.h"

struct Parser{
  Token **tokens;
  int ntokens;
  int ip;
};

Ast_node* parse(Token **tokens, int ntoken);
Ast_node* create_ast_node(node_type type);
Ast_node* expression();
Ast_node* term();
Ast_node* factor();
Ast_node* literal();

#endif // PARSER
