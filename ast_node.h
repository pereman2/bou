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
#define get_struct(node) node->value.statement.value.structs

#define get_expression(node) node->value.expression
#define get_statement(node) node->value.statement

typedef struct Ast_node Ast_node;

typedef enum {
  STATEMENT,
  EXPRESSION,

  // types of expressions
  BINARY,
  LITERAL,
  IDENTIFIER,

  // types of statements
  BLOCK,
  IF,
  FUNC,
  STRUCT
} node_type;

enum binary_type { ADD, SUB, DECL, ASSIGN, MULTI, DIV };

struct AstBinary {
  Ast_node* left;
  Ast_node* right;
  // TODO: declr and assign should be a statement
  // expression resolves to a value
  binary_type op;
};

struct AstIdentifier {
  Token token;
  Token type;
};

enum literal_type {
  INT,
  CHAR,
  FLOAT,
  BOOL,
};

struct AstLiteral {
  literal_type type;
  int value;
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

struct AstStruct {
  char* name;
  darray parameters;  // AstIdentifier
};

struct AstFunc {
  char* name;
  darray parameters;  // AstIdentifier
  Ast_node* block;
  Token return_type;
};

struct AstIf {
  // TODO: change with AstBlocks
  Ast_node* if_block;
  Ast_node* else_block;
  Ast_node* condition;
  darray parameters;
};

struct AstStatement {
  node_type type;
  union {
    AstExpression expression;
    AstBlock block;
    AstIf ifs;
    AstFunc func;
    AstStruct structs;
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

#define get_float_from_int(int_value) *(float*)(&int_value)
#define set_float_to_int(float_value) *(int*)&float_value
// todo: better naming
#define ast_do_binary_op_wrap(l, r, op)         \
  {                                             \
    if (l->type == FLOAT || r->type == FLOAT) { \
      float lhs = get_float_from_int(l->value); \
      if (l->type != literal_type::FLOAT) {     \
        lhs = (float)l->value;                  \
      }                                         \
      float rhs = get_float_from_int(r->value); \
      if (r->type != literal_type::FLOAT) {     \
        rhs = (float)r->value;                  \
      }                                         \
      float res = lhs op rhs;                   \
      return set_float_to_int(res);             \
    } else {                                    \
      return l->value op r->value;              \
    }                                           \
  }

int ast_do_binary_op(AstLiteral* left, AstLiteral* right, binary_type type);
