#pragma once

#include "token.h"

#define get_literal(node) node->expr.literal

#define get_binary(node) node->expr.binary
#define get_binary_left(node) node->expr.binary.left
#define get_binary_right(node) node->expr.binary.right

#define get_identifier(node) node->expr.identifier

typedef struct Ast_node Ast_node;

typedef enum { BINARY, LITERAL, IDENTIFIER } node_type;

struct AstBinary {
  Ast_node *left;
  Ast_node *right;
  enum { ADD, SUB, DECL, ASSIGN, MULTI, DIV } op;
};

struct AstIdentifier {
  Token *token;
  Token *type;
};

struct AstLiteral {
  enum {
    INT,
    CHAR,
    FLOAT,
    BOOL,
  } type;

  union {
    int i;
    char c;
    bool b;
    float f;
  } value;
};

struct Ast_node {
  node_type type;
  union {
    AstBinary binary;
    AstLiteral literal;
    AstIdentifier identifier;
  } expr;
};
