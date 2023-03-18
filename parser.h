#ifndef PARSER
#define PARSER

#include "ast_node.h"
#include "token.h"
#include <map>
#include <string>

struct Parser {
  Token **tokens;
  int ntokens;
  int ip;
  std::map<std::string, Token *> identifiers;
};

Ast_node *parse(Token **tokens, int ntoken);
Ast_node *create_ast_node(node_type type);

Ast_node *statement();
Ast_node *parse_block();
Ast_node *expression(bool semicolon = true);

Ast_node *decl();
Ast_node *ident();
Ast_node *assignment();

Ast_node *term();
Ast_node *factor();
Ast_node *literal();


bool is_type_id(int pos = 0);

#endif // PARSER
