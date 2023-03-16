#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ast_node.h"
#include "debug.h"
#include "parser.h"
#include "token.h"

Parser parser;

Ast_node *parse(Token **tokens, int ntokens) {
  // for now only one expression
  parser.tokens = tokens;
  parser.ntokens = ntokens;
  parser.ip = 0;
  Ast_node *ast = statement();
  print_ast(ast);
  return ast;
}

Ast_node *create_ast_node(node_type type) {
  Ast_node *node = (Ast_node *)malloc(sizeof(Ast_node));
  node->type = type;
  switch (type) {
  case LITERAL:
    node->type = node_type::EXPRESSION;
    get_expression(node).type = node_type::LITERAL;
    break;
  case BINARY:
    node->type = node_type::EXPRESSION;
    get_binary(node).left = NULL;
    get_binary(node).right = NULL;
    get_expression(node).type = node_type::BINARY;
    break;
  case IDENTIFIER:
    node->type = node_type::EXPRESSION;
    get_identifier(node).token = NULL;
    get_identifier(node).type = NULL;
    get_expression(node).type = node_type::IDENTIFIER;
    break;
  case BLOCK:
    init_darray(&get_block(node).statements);
    node->type = node_type::STATEMENT;
    get_statement(node).type = node_type::BLOCK;
    break;
  }
  return node;
}

static Token *peek() { return parser.tokens[parser.ip]; }

static Token *peek(int offset) { return parser.tokens[parser.ip + offset]; }

bool match(const char c) { return *peek()->start == c; }

bool match(const token_type type) { return peek()->type == type; }

bool match(const token_type type, int offset) {
  return peek(offset)->type == type;
}

Token *next_token() { return parser.tokens[parser.ip++]; }

bool expect(const token_type type) { return next_token()->type == type; }

Ast_node *statement() {
  switch(peek()->type) {
    case T_LEFT_BRACE:
      {
        next_token();
        Ast_node *node = create_ast_node(node_type::BLOCK);
        while (!match(T_RIGHT_BRACE) && !match(T_EOF)) {
          Ast_node *stmt = statement();
          darray_push(&get_block(node).statements, sizeof(Ast_node), (void*)stmt);
        };
        expect(T_RIGHT_BRACE);
        return node;
      }
    default:
      {
        return expression();
      }
  }
}
Ast_node *expression() {
  Ast_node *r =  decl(); 
  assert(expect(T_SEMICOLON));
  return r;
}

std::string get_identifier_name(Token *token) {
  assert(token->type == T_IDENTIFIER);
  std::string name(token->start, token->end - token->start);
  return name;
}

Ast_node *create_assign(int op, Token *ident, Token *type) {
  assert(ident != NULL);
  assert(type != NULL);

  Ast_node *node = create_ast_node(BINARY);
  get_binary(node).op = AstBinary::DECL;
  get_binary(node).left = create_ast_node(IDENTIFIER);
  get_identifier(get_binary_left(node)).token = ident;

  std::string ident_name = get_identifier_name(ident);
  if (op == AstBinary::DECL) {
    parser.identifiers.emplace(ident_name, type);
  }
  get_identifier(get_binary_left(node)).type = parser.identifiers[ident_name];
  return node;
}

Ast_node *decl() {
  if (match(T_IDENTIFIER) && match(T_COLON, 1)) {
    Token *ident = next_token();
    next_token();
    if (match(T_BOOLID) || match(T_FLOATID) || match(T_INTID) ||
        match(T_CHARID)) {
      Token *type = next_token();
      if (expect(T_EQUAL)) {
        Ast_node *node = create_assign(AstBinary::DECL, ident, type);
        get_binary_right(node) = term();
        return node;
      }
    }
  }
  return assignment();
}

Ast_node *assignment() {
  if (match(T_IDENTIFIER) && match(T_EQUAL, 1)) {
    Token *ident = next_token();
    next_token();
    auto ident_it = parser.identifiers.find(get_identifier_name(ident));
    if (ident_it == parser.identifiers.end()) {
      printf("Error, identifier %s isn't declared\n",
             get_identifier_name(ident).c_str());
      exit(1);
    }
    Ast_node *node =
        create_assign(AstBinary::ASSIGN, ident,
                      parser.identifiers[get_identifier_name(ident)]);
    get_binary_right(node) = term();
    return node;
  }
  return term();
}

Ast_node *term() {
  Ast_node *t = factor();

  Token *p = peek();

  Ast_node *bin = t;
  Ast_node *previous = NULL;

  bool added = 0;
  while (match('-') || match('+')) {
    parser.ip++; // peek successful
    previous = bin;
    bin = create_ast_node(node_type::BINARY);
    get_binary(bin).left = previous;
    if (p->type == T_MINUS) {
      get_binary(bin).op = AstBinary::SUB;
    } else {
      get_binary(bin).op = AstBinary::ADD;
    }
    get_binary(bin).right = factor();

    added = 1;
  }
  if (added) {
    return bin;
  }
  return t;
}

Ast_node *factor() { return literal(); }

Ast_node *literal() {
  Ast_node *l = create_ast_node(node_type::LITERAL);
  Token *p = next_token();
  switch (p->type) {
  case T_CHAR:
    get_literal(l).value.c = *(p->start);
    get_literal(l).type = AstLiteral::CHAR;
    break;
  case T_INT:
    get_literal(l).value.i = (int)atoi(p->start);
    get_literal(l).type = AstLiteral::INT;
    break;
  case T_FLOAT:
    get_literal(l).value.f = (float)atof(p->start);
    get_literal(l).type = AstLiteral::FLOAT;
    break;
  case T_BOOL:
    if (strncmp("true", p->start, 4) == 0) {
      get_literal(l).value.b = true;
    } else if (strncmp("false", p->start, 5) == 0) {
      get_literal(l).value.b = false;
    } else {
      printf("unexpected bool token %s\n", p->start);
      exit(1);
    }
    break;
  default:
    printf("Error parsing literal");
    print_token(p);
    exit(1);
  }
  return l;
}
