typedef struct Ast_node Ast_node;
typedef struct Ast_sum_op Ast_sum_op;
typedef struct Ast_literal Ast_literal;
struct Ast_node {
  enum { Ast_sum_op sum_op; Ast_literal literal; }
  expr;
};

struct Ast_sum_op {
  enum { SUM, SUB } type;
};

struct Ast_literal {
  enum { INT, FLOAT, CHAR, STRING, BOOL } type;
  char* value;
};

struct Ast_sum_expr {
  Ast_node left;
  Ast_node right;
  Ast_node op;
};
