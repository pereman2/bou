#include <cstdlib>
#include <cstdio>
#include <assert.h>

#include "ast_node.h"
#include "token.h"
#include "debug.h"
#include "parser.h"


Parser parser;

Ast_node* parse(Token **tokens, int ntokens) {
  // for now only one expression
  parser.tokens = tokens;
  parser.ntokens = ntokens;
  parser.ip = 0;
  Ast_node *ast = expression();
  print_ast(ast);
  return ast;
}

Ast_node* create_ast_node(node_type type) {
  Ast_node *node = (Ast_node*) malloc(sizeof(Ast_node));
  node->type = type;
  switch (type) {
    case LITERAL:
      node->expr.literal = (Ast_literal*) malloc(sizeof(Ast_literal));
      node->type = node_type::LITERAL;
      break;
    case BINARY:
      node->expr.binary = (Ast_binary*) malloc(sizeof(Ast_binary));
      node->expr.binary->left = NULL;
      node->expr.binary->right = NULL;
      node->type = node_type::BINARY;
      node->expr.binary->res_in_register = false;
      break;
    case IDENTIFIER:
      identifier(node) = (Ast_identifier*) malloc(sizeof(Ast_identifier));
      identifier(node)->token = NULL;
      identifier(node)->type = NULL;
  }
  return node;
}

static Token *peek() {
  return parser.tokens[parser.ip];
}

bool match(const char c) {
  return *peek()->start == c;
}

bool match(const token_type type) {
  return peek()->type == type;
}


Token *next_token() {
  return parser.tokens[parser.ip++];
}

bool expect(const token_type type) {
  return next_token()->type == type;
}

Ast_node *expression() {
  return decl();
}

std::string get_identifier_name(Token *token) {
  assert(token->type == T_IDENTIFIER);
  std::string name(token->start, token->end - token->start);
  return name;

}


Ast_node *decl() {
  if (match(T_IDENTIFIER)) {
    Token *ident = next_token();
    if (match(T_COLON)) {
      next_token();
      if (match(T_BOOLID) || match(T_FLOATID) || match(T_INTID) || match(T_CHARID)) {
        Token *type = next_token();
        if (expect(T_EQUAL)) {
          Ast_node *node = create_ast_node(BINARY);
          node->expr.binary->op = Ast_binary::DECL;
          node->expr.binary->left = create_ast_node(IDENTIFIER);
          identifier(get_binary_left(node))->token = ident;
          parser.identifiers.emplace(get_identifier_name(ident), type);
          identifier(get_binary_left(node))->type = parser.identifiers[get_identifier_name(ident)];
          get_binary_right(node) = term();
          return node;
        }
      }
    }
  }
  return assignment();
}

Ast_node *assignment() {
  if (match(T_IDENTIFIER)) {
    Token *ident = next_token();
    if (match(T_EQUAL)) {
      next_token();
      Ast_node *node = create_ast_node(BINARY);
      node->expr.binary->op = Ast_binary::ASSIGN;
      node->expr.binary->left = create_ast_node(IDENTIFIER);
      identifier(get_binary_left(node))->token = ident;
      identifier(get_binary_left(node))->type = parser.identifiers[get_identifier_name(ident)];
      get_binary_right(node) = term();
      return node;
    }
    printf("error identifier\n");
    exit(1);
  }
  return term();
}


Ast_node *term() {
  Ast_node *t = factor();

  Token *p = peek();

  Ast_node *bin = t;
  Ast_node *previous = NULL;

  bool added = 0;
  while (match('-')|| match('+')) {
    parser.ip++; // peek successful
    previous = bin;
    bin = create_ast_node(node_type::BINARY);
    bin->expr.binary->left = previous;
    if (p->type == T_MINUS) {
      bin->expr.binary->op = Ast_binary::SUB;
    } else {
      bin->expr.binary->op = Ast_binary::ADD;
    }
    bin->expr.binary->right = factor();
    bin->expr.binary->res_in_register = true;

    added = 1;
  }
  bin->expr.binary->res_in_register = false;
  if(added) {
    return bin;
  }
  return t;
}

Ast_node *factor() {
  return literal();
}

Ast_node *literal() {
  Ast_node *l = create_ast_node(node_type::LITERAL);
  Token *p = next_token();
  switch(p->type) {
    case T_CHAR:
      l->expr.literal->value.c = *(p->start);
      break;
    case T_INT:
      l->expr.literal->value.i = (int)atoi(p->start);
      break;
    // case T_FLOAT:
    //   l->expr.literal->value.f = *(p->start);
    //   break;
    // case T_BOOL:
    //   l->expr.literal->value.b = *(p->start);
    //   break;
    // case T_STRING:
    //   l->expr.literal->value.s = *(p->start);
    //   break;
    default:
      printf("Error parsing literal");
      print_token(p);
      exit(1);
  }
  return l;
}
