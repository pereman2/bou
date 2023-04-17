#pragma once

#include "darray.h"
#include "token.h"
#include "util.h"

#define get_literal(node) node->value.expression.expr.literal
#define get_unary(node) node->value.expression.expr.unary

#define get_binary(node) node->value.expression.expr.binary
#define get_binary_left(node) node->value.expression.expr.binary.left
#define get_binary_right(node) node->value.expression.expr.binary.right

#define get_identifier(node) node->value.expression.expr.identifier
#define get_block(node) node->value.statement.value.block
#define get_if(node) node->value.statement.value.ifs
#define get_func(node) node->value.statement.value.func
#define get_struct(node) node->value.statement.value.structs
#define get_union(node) node->value.statement.value.union_
#define get_enum(node) node->value.statement.value.enum_

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
  UNARY,

  // types of statements
  BLOCK,
  IF,
  FUNC,
  STRUCT,
  UNION,
  ENUM
} node_type;

enum binary_type {
  ADD,
  SUB,
  DECL,
  ASSIGN,
  MULT,
  DIV,

  EQUAL_EQUAL,
  LESS,
  LESS_EQUAL,
  GREATER_EQUAL,
  GREATER,

  AND,
  OR

};

struct AstBinary {
  Ast_node* left;
  Ast_node* right;
  // TODO: declr and assign should be a statement
  // expression resolves to a value
  binary_type op;
};

enum value_type {
  CHAR, INT, FLOAT, BOOL, IDENT 
};

struct AstType {
  value_type value;
  char *ident_name;
  bool is_pointer;
};

struct AstIdentifier {
  Token token;
  AstType type;
};

struct AstLiteral {
  value_type type;
  int value;
};

enum unary_type {
  DEREF, GET_PTR, NEGATE
};

struct AstUnary {
  unary_type type;
   // TODO: we need to check wether we can deref or get_ptr based on the expected type from the evaluation of the ast_node
   // if a is not a pointer it should fail.
   // *(a + (b + 10)
  Ast_node *node;
};

struct AstExpression {
  node_type type;
  AstType evaluates_to;
  union {
    AstUnary unary;
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
  AstType return_type;
};

struct AstIf {
  // TODO: change with AstBlocks
  Ast_node* if_block;
  Ast_node* else_block;
  Ast_node* condition;
  darray parameters;
};

struct AstUnion {
  char* name;
  darray parameters;  // AstIdentifier
};


struct AstEnumParameter {
  int value;
  bou_string name;
};

struct AstEnum {
  bou_string name;
  darray enum_parameters; // Type: enum_parameter
};

struct AstStatement {
  node_type type;
  union {
    AstExpression expression;
    AstBlock block;
    AstIf ifs;
    AstFunc func;
    AstStruct structs;
    AstUnion union_;
    AstEnum enum_;
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
      if (l->type != value_type::FLOAT) {     \
        lhs = (float)l->value;                  \
      }                                         \
      float rhs = get_float_from_int(r->value); \
      if (r->type != value_type::FLOAT) {     \
        rhs = (float)r->value;                  \
      }                                         \
      float res = lhs op rhs;                   \
      return set_float_to_int(res);             \
    } else {                                    \
      return l->value op r->value;              \
    }                                           \
  }

int ast_do_binary_op(AstLiteral* left, AstLiteral* right, binary_type type);
