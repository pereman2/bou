#pragma once

#include "darray.h"
#include "token.h"

#define get_literal(node) node->value.expression.expr.literal

#define get_binary(node) node->value.expression.expr.binary
#define get_binary_left(node) node->value.expression.expr.binary.left
#define get_binary_right(node) node->value.expression.expr.binary.right

#define get_identifier(node) node->value.expression.expr.identifier
#define get_block(node) node->value.statement.value.block

#define get_expression(node) node->value.expression
#define get_statement(node) node->value.statement

typedef struct Ast_node Ast_node;

typedef enum { 
  STATEMENT, EXPRESSION ,
  BINARY, LITERAL, IDENTIFIER ,
  BLOCK, STATEMENT_EXPRESSION,
} node_type;

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

struct AstExpression {
  node_type type;
  union {
    AstBinary binary;
    AstLiteral literal;
    AstIdentifier identifier;
  } expr;
};

struct AstBlock {
  darray statements;
};

struct AstStatement {
  node_type type;
  union {
    AstExpression expression;
    AstBlock block;
    // blocks,if,loop...
  } value;
};

struct Ast_node {
  node_type type;
  union {
    AstExpression expression;
    AstStatement statement;
  } value;
};
