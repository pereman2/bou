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
  case IF:
    node->type = node_type::STATEMENT;
    get_statement(node).type = node_type::IF;
    get_if(node).if_block = NULL;
    get_if(node).else_block = NULL;
    break;
  case FUNC:
    init_darray(&get_func(node).parameters);
    node->type = node_type::STATEMENT;
    get_statement(node).type = node_type::FUNC;
    get_func(node).block = NULL;
    break;
  }
  return node;
}

static Token *peek() { return parser.tokens[parser.ip]; }

void ignore_lf() {
  while (parser.tokens[parser.ip]->type == T_LF) {
    parser.ip++;
  }
}

static Token *peek(int offset) { 
  ignore_lf();
  return parser.tokens[parser.ip + offset]; 
}

bool match(const char c) { 
  ignore_lf();
  return *peek()->start == c; 
}

bool match(const token_type type) { 
  ignore_lf();
  return peek()->type == type; 
}

bool match(const token_type type, int offset) {
  ignore_lf();
  return peek(offset)->type == type;
}

Token *next_token() { 
  ignore_lf();
  return parser.tokens[parser.ip++]; 
}

bool expect(const token_type type) { return next_token()->type == type; }

#define assert_parser(expr) \
  if (!(expr)) { \
    printf("------- token trace ----\n"); \
    print_token(peek(0)); \
    print_token(peek(1)); \
    print_token(peek(2)); \
    assert(expr); \
  } \


AstParameter get_ast_parameter() {
  AstParameter param;
  if(match(T_IDENTIFIER, 0) && match(T_COLON, 1) && is_type_id(2)) {
    param.token = *peek();
    next_token();
    next_token();
    param.type = *peek();
    next_token();
    return param;
  }
  printf("not a parameter\n");
  print_token(peek());
  print_token(peek(1));
  print_token(peek(2));
  exit(1);
}

Ast_node *statement() {
  switch(peek()->type) {
    case T_LEFT_BRACE:
      {
        return parse_block();
      }
    case T_IF:
      {
        next_token();
        Ast_node *node = create_ast_node(node_type::IF);
        expect(T_LEFT_PAR);
        get_if(node).condition = expression(false);
        expect(T_RIGHT_PAR);
        get_if(node).if_block = parse_block();
        if (match(T_ELSE)) {
          next_token();
          get_if(node).else_block = parse_block();
        }
        return node;
      }
     case T_DEF:
      {
        next_token();
        Ast_node *node = create_ast_node(node_type::FUNC);
        assert_parser(match(T_IDENTIFIER));
        Token *func_name = peek();
        size_t name_size = func_name->end - func_name->start;
        get_func(node).name = (char*)malloc(name_size+1);
        memcpy(get_func(node).name, func_name->start, name_size);
        get_func(node).name[name_size] = '\0';

        next_token();
        assert_parser(expect(T_LEFT_PAR));

        while(match(T_IDENTIFIER)) {
          AstParameter param = get_ast_parameter();
          darray_push(&get_func(node).parameters, sizeof(AstParameter), &param);
          if (match(T_COMMA)) {
            next_token();
          }
        }
        assert_parser(expect(T_RIGHT_PAR));
        assert_parser(expect(T_COLON));
        assert_parser(is_type_id());
        Token *type = peek();
        copy_token(&get_func(node).return_type, type);
        next_token();

        get_func(node).block = parse_block();
        return node;
      }
    default:
      {
        return expression();
      }
  }
}

Ast_node *parse_block() {
  assert_parser(expect(T_LEFT_BRACE));
  Ast_node *node = create_ast_node(node_type::BLOCK);
  while (!match(T_RIGHT_BRACE) && !match(T_EOF)) {
    Ast_node *stmt = statement();
    darray_push(&get_block(node).statements, sizeof(Ast_node), (void*)stmt);
  };
  assert_parser(expect(T_RIGHT_BRACE));
  return node;
}

Ast_node *expression(bool semicolon) {
  Ast_node *r =  decl(); 
  if (semicolon) {
    assert_parser(expect(T_SEMICOLON));
  }
  return r;
}

std::string get_identifier_name(Token *token) {
  assert_parser(token->type == T_IDENTIFIER);
  std::string name(token->start, token->end - token->start);
  return name;
}

Ast_node *create_assign(int op, Token *ident, Token *type) {
  assert_parser(ident != NULL);
  assert_parser(type != NULL);

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
    if (is_type_id()) {
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
    next_token(); // peek successful
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
    print_token(peek(-2));
    print_token(peek(-1));
    print_token(peek(0));
    print_token(peek(1));
    print_token(peek(2));
    print_token(peek(3));
    exit(1);
  }
  return l;
}

bool is_type_id(int pos) {
  return match(T_BOOLID, pos) || match(T_FLOATID, pos) || match(T_INTID, pos) ||
    match(T_CHARID, pos);
}
