#include "parser.h"

#include <assert.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "arena.h"
#include "ast_node.h"
#include "debug.h"
#include "token.h"
#include "util.h"


Parser parser;

Ast_node* parse(Token** tokens, int ntokens) {
  // for now only one expression
  parser.tokens = tokens;
  parser.ntokens = ntokens;
  parser.ip = 0;
  parser.arena = arena_create_and_init(MB(4));
  printf("%d arena size\n", MB(4));
  Ast_node* ast = statement();
  print_ast(ast);
  return ast;
}

Ast_node* create_ast_node(node_type type) {
  Ast_node* node = (Ast_node*)arena_allocate(parser.arena, sizeof(Ast_node));
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
    case UNARY:
      node->type = node_type::EXPRESSION;
      get_unary(node).node = NULL;
      get_expression(node).type = node_type::UNARY;
      break;
    case IDENTIFIER:
      node->type = node_type::EXPRESSION;
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
    case STRUCT:
      init_darray(&get_struct(node).parameters);
      node->type = node_type::STATEMENT;
      get_statement(node).type = node_type::STRUCT;
      break;
  }
  return node;
}

static Token* peek() { return parser.tokens[parser.ip]; }

void ignore_lf() {
  while (parser.tokens[parser.ip]->type == T_LF) {
    parser.ip++;
  }
}

static Token* peek(int offset) {
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

Token* next_token() {
  ignore_lf();
  return parser.tokens[parser.ip++];
}

bool expect(const token_type type) { return next_token()->type == type; }

void ast_type_from_token(AstType *type, Token *token) {
  switch(token->type) {
    case T_INT:
      type->value = value_type::INT;
      break;
    case T_BOOL:
      type->value = value_type::BOOL;
      break;
    case T_CHAR:
      type->value = value_type::CHAR;
      break;
    case T_FLOAT:
      type->value = value_type::FLOAT;
      break;
    case T_INTID:
      type->value = value_type::INT;
      break;
    case T_BOOLID:
      type->value = value_type::BOOL;
      break;
    case T_CHARID:
      type->value = value_type::CHAR;
      break;
    case T_FLOATID:
      type->value = value_type::FLOAT;
      break;
    case T_IDENTIFIER:
      type->value = value_type::IDENT;
      break;
  }
  uint32_t type_size = token->end - token->start;
  type->ident_name = (char*)arena_allocate(parser.arena, type_size+1);
  memcpy(type->ident_name, token->start, type_size);
  type->ident_name[type_size] = '\0';
}

#define assert_parser(expr)               \
  if (!(expr)) {                          \
    printf("------- -1 token trace ----\n"); \
    print_token(peek(-3));                 \
    print_token(peek(-2));                 \
    print_token(peek(-1));                 \
    printf("------- token trace ----\n"); \
    print_token(peek(0));                 \
    print_token(peek(1));                 \
    print_token(peek(2));                 \
    assert(expr);                         \
  }

AstIdentifier get_ast_parameter() {
  AstIdentifier param;
  param.type.is_pointer = false;
  if (match(T_IDENTIFIER, 0) && match(T_COLON, 1) && is_type_id(2)) {
    param.token = *peek();
    next_token();
    next_token();
    ast_type_from_token(&param.type, peek());
    next_token();
    if (match(T_STAR)) {
      param.type.is_pointer = true;
      next_token();
    }
    return param;
  }
  printf("not a parameter\n");
  print_token(peek());
  print_token(peek(1));
  print_token(peek(2));
  exit(1);
}

Ast_node* statement() {
  switch (peek()->type) {
    case T_LEFT_BRACE: {
      return parse_block();
    }
    case T_IF: {
      next_token();
      Ast_node* node = create_ast_node(node_type::IF);
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
    case T_DEF: {
      next_token();
      Ast_node* node = create_ast_node(node_type::FUNC);
      assert_parser(match(T_IDENTIFIER));
      Token* func_name = peek();
      size_t name_size = func_name->end - func_name->start;
      get_func(node).name = (char*)arena_allocate(parser.arena, name_size + 1);
      memcpy(get_func(node).name, func_name->start, name_size);
      get_func(node).name[name_size] = '\0';

      next_token();
      assert_parser(expect(T_LEFT_PAR));

      while (match(T_IDENTIFIER)) {
        AstIdentifier param = get_ast_parameter();
        darray_push(&get_func(node).parameters, sizeof(AstIdentifier), &param);
        if (match(T_COMMA)) {
          next_token();
        }
      }
      assert_parser(expect(T_RIGHT_PAR));
      assert_parser(expect(T_COLON));
      assert_parser(is_type_id());
      Token* type = peek();
      get_func(node).return_type.is_pointer = false;
      if (match(T_STAR)) {
        get_func(node).return_type.is_pointer = true;
        next_token();
      }
      ast_type_from_token(&get_func(node).return_type, type);
      next_token();

      get_func(node).block = parse_block();
      return node;
    }

    case T_STRUCT: {
      next_token();
      Ast_node* node = create_ast_node(node_type::STRUCT);

      assert_parser(match(T_IDENTIFIER));
      Token* func_name = peek();
      size_t name_size = func_name->end - func_name->start;
      get_struct(node).name = (char*)arena_allocate(parser.arena, name_size + 1);
      memcpy(get_struct(node).name, func_name->start, name_size);
      get_struct(node).name[name_size] = '\0';
      next_token();
      assert_parser(expect(T_LEFT_BRACE));

      while (match(T_IDENTIFIER)) {
        AstIdentifier id = get_ast_parameter();
        darray_push(&get_struct(node).parameters, sizeof(AstIdentifier), &id);
        assert_parser(expect(T_SEMICOLON));
      }

      assert_parser(expect(T_RIGHT_BRACE));

      return node;
    }
    default: {
      return expression();
    }
  }
}

Ast_node* parse_block() {
  assert_parser(expect(T_LEFT_BRACE));
  Ast_node* node = create_ast_node(node_type::BLOCK);
  while (!match(T_RIGHT_BRACE) && !match(T_EOF)) {
    Ast_node* stmt = statement();
    darray_push(&get_block(node).statements, sizeof(Ast_node), (void*)stmt);
  };
  assert_parser(expect(T_RIGHT_BRACE));
  return node;
}

Ast_node* expression(bool semicolon) {
  Ast_node* r = decl();
  if (semicolon) {
    assert_parser(expect(T_SEMICOLON));
  }
  return r;
}

std::string get_identifier_name(Token* token) {
  assert_parser(token->type == T_IDENTIFIER);
  std::string name(token->start, token->end - token->start);
  return name;
}

AstType binary_evaluates_to(AstType* left, AstType* right) {
  AstType type;
  if (left->value == value_type::IDENT || right->value == value_type::IDENT) {
    printf("binary ops on identifiers are not supported yet!\n");
    exit(1);
  }

  if (left->is_pointer && right->is_pointer) {
    printf("binary ops on two pointers are not supported yet!\n");
    exit(1);
  }

  if (left->value == value_type::FLOAT || right->value == value_type::FLOAT) {
    type.value = value_type::FLOAT; 
  } else if (left->value == value_type::INT || right->value == value_type::INT) {
    type.value = value_type::INT; 
  } else if (left->value == value_type::CHAR || right->value == value_type::CHAR) {
    type.value = value_type::CHAR; 
  } else if (left->value == value_type::BOOL || right->value == value_type::BOOL) {
    type.value = value_type::BOOL; 
  } else {
    printf("Uknonwn evaluation\n");
    exit(1);
  }
  if (left->is_pointer || right->is_pointer) {
    type.is_pointer = true;
  }
  return type;
}

Ast_node* create_assign(int op, Token* ident, AstType type) {
  assert_parser(ident != NULL);

  Ast_node* node = create_ast_node(BINARY);
  get_binary(node).op = binary_type::DECL;
  get_binary(node).left = create_ast_node(IDENTIFIER);
  copy_token(&get_identifier(get_binary_left(node)).token, ident);

  std::string ident_name = get_identifier_name(ident);
  get_identifier(get_binary_left(node)).type = type;
  if (op == binary_type::DECL) {
    parser.identifiers.emplace(ident_name, get_identifier(get_binary_left(node)));
  }
  get_identifier(get_binary_left(node)) = parser.identifiers[ident_name];
  return node;
}

Ast_node* decl() {
  if (match(T_IDENTIFIER) && match(T_COLON, 1)) {
    Token* ident = next_token();
    next_token();
    if (is_type_id()) {
      Token* type_token = next_token();
      AstType type;
      type.is_pointer = false;
      ast_type_from_token(&type, type_token);
      if (match(T_STAR)) {
        type.is_pointer = true;
        next_token();
      }
      assert_parser(expect(T_EQUAL));
      Ast_node* node = create_assign(binary_type::DECL, ident, type);
      get_binary_right(node) = comparison();
      return node;
    }
  }
  return comparison();
}

AstIdentifier lookup_identifier(Token *ident_token) {
  return parser.identifiers[get_identifier_name(ident_token)];
}

Ast_node* comparison() {
  Ast_node *left = assignment();
  if (match(T_EQUAL_EQUAL) || match(T_LESS) || match(T_LESS_EQUAL)
      || match(T_GREATER) || match(T_GREATER_EQUAL)) {
    Ast_node *node = create_ast_node(node_type::BINARY);
    get_expression(node).evaluates_to.is_pointer = false;
    get_expression(node).evaluates_to.value = value_type::BOOL;
    get_binary(node).left = left;
    if (match(T_EQUAL_EQUAL)) {
      get_binary(node).op = binary_type::EQUAL_EQUAL;
    } else if (match(T_LESS)) {
      get_binary(node).op = binary_type::LESS;
    } else if (match(T_GREATER_EQUAL)) {
      get_binary(node).op = binary_type::GREATER_EQUAL;
    } else if (match(T_LESS_EQUAL)) {
      get_binary(node).op = binary_type::LESS_EQUAL;
    } else if (match(T_GREATER)) {
      get_binary(node).op = binary_type::GREATER;
    } else {
      assert_parser(false);
    }
    next_token(); 
    get_binary(node).right = comparison();
    return node;
  }
  return left;
}

Ast_node* assignment() {
  if (match(T_IDENTIFIER) && match(T_EQUAL, 1)) {
    Token* ident = next_token();
    next_token();
    auto ident_it = parser.identifiers.find(get_identifier_name(ident));
    if (ident_it == parser.identifiers.end()) {
      printf("Error, identifier %s isn't declared\n", get_identifier_name(ident).c_str());
      exit(1);
    }
    Ast_node* node =
        create_assign(binary_type::ASSIGN, ident, lookup_identifier(ident).type);
    get_binary_right(node) = term();
    get_expression(node).evaluates_to = get_expression(get_binary_right(node)).evaluates_to;
    return node;
  }
  return term();
}

Ast_node* term() {
  Ast_node* t = factor();

  Token* p = peek();

  Ast_node* bin = t;
  Ast_node* previous = NULL;

  bool added = 0;
  while (match('-') || match('+')) {
    next_token();  // peek successful
    previous = bin;
    bin = create_ast_node(node_type::BINARY);
    get_binary(bin).left = previous;
    if (p->type == T_MINUS) {
      get_binary(bin).op = binary_type::SUB;
    } else {
      get_binary(bin).op = binary_type::ADD;
    }
    get_binary(bin).right = factor();

    get_expression(bin).evaluates_to =
      binary_evaluates_to(&get_expression(get_binary(bin).left).evaluates_to,
                          &get_expression(get_binary(bin).right).evaluates_to);

    if (get_binary(bin).left->type == node_type::EXPRESSION &&
        get_binary(bin).right->type == node_type::EXPRESSION &&
        get_expression(get_binary(bin).left).type == node_type::LITERAL &&
        get_expression(get_binary(bin).right).type == node_type::LITERAL) {
      bin->type = node_type::EXPRESSION;
      Ast_node* right = get_binary(bin).right;
      value_type type = get_literal(get_binary(bin).left).type;
      get_literal(bin).value =
          ast_do_binary_op(&get_literal(get_binary(bin).left), &get_literal(get_binary(bin).right),
                           get_binary(bin).op);
      get_literal(bin).type = type;
      get_expression(bin).type = node_type::LITERAL;
    }

    added = 1;
  }
  if (added) {
    return bin;
  }
  return t;
}

Ast_node* factor() { return deref(); }

Ast_node* deref() {
  if (match(T_STAR)) {
    next_token();
    Ast_node *node = create_ast_node(node_type::UNARY);
    get_unary(node).type = unary_type::DEREF;
    get_unary(node).node = ref();
    assert_parser(get_expression(get_unary(node).node).evaluates_to.is_pointer == true);
    return node;
  }
  return ref();
}

Ast_node* ref() {
  if (match(T_AMPERSAND)) {
    next_token();
    Ast_node *node = create_ast_node(node_type::UNARY);
    get_unary(node).type = unary_type::GET_PTR;
    get_unary(node).node = literal();
    assert_parser(get_expression(get_unary(node).node).evaluates_to.is_pointer == false);
    return node;
  }
  return literal();
}

Ast_node* literal() {
  Ast_node* l = create_ast_node(node_type::LITERAL);
  Token* p = next_token();
  AstType type;
  type.is_pointer = false;
  switch (p->type) {
    case T_CHAR:
      get_literal(l).value = *(p->start);
      get_literal(l).type = value_type::CHAR;
      type.value = value_type::CHAR;
      break;
    case T_INT:
      get_literal(l).value = (int)atoi(p->start);
      get_literal(l).type = value_type::INT;
      type.value = value_type::INT;
      break;
    case T_FLOAT: {
      float f = atof(p->start);
      get_literal(l).value = *(int*)&f;
      get_literal(l).type = value_type::FLOAT;
      type.value = value_type::FLOAT;
      break;
    }
    case T_BOOL:
      if (strncmp("true", p->start, 4) == 0) {
        get_literal(l).value = 1;
      } else if (strncmp("false", p->start, 5) == 0) {
        get_literal(l).value = 0;
      } else {
        printf("unexpected bool token %s\n", p->start);
        exit(1);
      }
      get_literal(l).type = value_type::BOOL;
      type.value = value_type::BOOL;
      break;
    case T_IDENTIFIER:
      {
        Ast_node* ident = create_ast_node(node_type::IDENTIFIER);
        get_identifier(ident) = lookup_identifier(p);
        get_expression(ident).evaluates_to = get_identifier(ident).type;
        return ident;
      }
    default:
      printf("Error parsing literal");
      print_token(peek(0));
      print_token(peek(1));
      print_token(peek(2));
      print_token(peek(3));
      exit(1);
  }
  get_expression(l).evaluates_to = type;
  return l;
}

bool is_type_id(int pos) {
  return match(T_BOOLID, pos) || match(T_FLOATID, pos) || match(T_INTID, pos) ||
         match(T_CHARID, pos);
}

int ast_do_binary_op(AstLiteral* left, AstLiteral* right, binary_type type) {
  switch (type) {
    case binary_type::ADD: {
      ast_do_binary_op_wrap(left, right, +);
    }
    case SUB: {
      ast_do_binary_op_wrap(left, right, -);
    }
    case MULTI: {
      ast_do_binary_op_wrap(left, right, *);
    }
    case DIV: {
      ast_do_binary_op_wrap(left, right, /);
    }
    case DECL: {
      printf("error binary op decl cannot be pre executed\n");
      exit(1);
    }
    case ASSIGN: {
      printf("error binary op assign cannot be pre executed\n");
      exit(1);
    }
  }
  return -1;
}
