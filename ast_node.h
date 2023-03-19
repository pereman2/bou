#pragma once

#include "darray.h"
#include "token.h"

#define get_literal(node) node->value.expression.expr.literal

#define get_binary(node) node->value.expression.expr.binary
#define get_binary_left(node) node->value.expression.expr.binary.left
#define get_binary_right(node) node->value.expression.expr.binary.right

#define get_identifier(node) node->value.expression.expr.identifier
#define get_block(node) node->value.statement.value.block
#define get_if(node) node->value.statement.value.ifs
#define get_func(node) node->value.statement.value.func

#define get_expression(node) node->value.expression
#define get_statement(node) node->value.statement

typedef struct Ast_node Ast_node;

typedef enum { 
  STATEMENT, EXPRESSION ,

  // types of expressions
  BINARY, LITERAL, IDENTIFIER,

  // types of statements
  BLOCK, IF, FUNC
} node_type;

struct AstBinary {
  Ast_node *left;
  Ast_node *right;
  // TODO: declr and assign should be a statement
  // expression resolves to a value
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

struct AstParameter {
  Token token;
  Token type;
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

struct AstFunc {
  char *name;
  darray parameters;
  Ast_node* block;
  Token return_type;
};

struct AstIf {
  // TODO: change with AstBlocks
  Ast_node *if_block;
  Ast_node *else_block;
  Ast_node *condition;
  darray parameters;
};

struct AstStatement {
  node_type type;
  union {
    AstExpression expression;
    AstBlock block;
    AstIf ifs;
    AstFunc func;
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
