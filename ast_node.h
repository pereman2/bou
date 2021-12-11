#pragma once

#define get_literal(node) node->expr.literal

#define get_binary(node) node->expr.binary

typedef struct Ast_node Ast_node; 
typedef struct Ast_binary Ast_binary; 
typedef struct Ast_literal Ast_literal; 

typedef enum {
	BINARY,
	LITERAL,
} node_type;

struct Ast_node {
	node_type type;
	union {
		Ast_binary *binary;
		Ast_literal *literal;
	} expr;
};

struct Ast_binary {
	Ast_node *left; 
	Ast_node *right; 
	enum {ADD, SUB} op;
};

struct Ast_literal {
	union {
		int i;
		char c;
		/* bool b;  */
		float f;
		/* string s;  */
	} value;
};
