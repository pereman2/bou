#include <cassert>

#include "ast_node.h"
#include "darray.h"
#include "debug.h"
#include "parser.h"
#include "test_util.h"
#include "token.h"

struct TokenState {
  Token* tokens[1024];
  int token_ip;
};

TokenState gstate;

Ast_node* parse(TokenState* state) { return parse(state->tokens, state->token_ip); }

void create_token(TokenState* state, token_type type, const char* value) {
  state->tokens[state->token_ip]->type = type;
  state->tokens[state->token_ip]->start = (char*)value;
  state->tokens[state->token_ip]->end = (char*)value + strlen(value);
  state->tokens[state->token_ip]->line = 0;
  state->token_ip++;
}

void init_token_state(TokenState* state) {
  for (int i = 0; i < 1024; i++) {
    state->tokens[i] = (Token*)malloc(sizeof(Token));
  }
  state->token_ip = 0;
}

void clear_token_state(TokenState* state) { state->token_ip = 0; }


void test_comparison() {
  create_token(&gstate, T_INT, "16");
  create_token(&gstate, T_LESS, "<");
  create_token(&gstate, T_INT, "3243");
  create_token(&gstate, T_SEMICOLON, ";");
  Ast_node* root = parse(&gstate);
  assert(get_binary(root).op == binary_type::LESS);
  assert(get_literal(get_binary(root).left).value == 16);
  assert(get_literal(get_binary(root).right).value == 3243);
  assert(get_expression(root).evaluates_to.value == value_type::BOOL);
}

void test_if() {
  create_token(&gstate, T_IF, "if");
  create_token(&gstate, T_LEFT_PAR, "(");

  create_token(&gstate, T_INT, "16");
  create_token(&gstate, T_LESS, "<");
  create_token(&gstate, T_INT, "3243");

  create_token(&gstate, T_RIGHT_PAR, ")");
  create_token(&gstate, T_LEFT_BRACE, "{");

  create_token(&gstate, T_IDENTIFIER, "foo");
  create_token(&gstate, T_COLON, ":");
  create_token(&gstate, T_INTID, "int");
  create_token(&gstate, T_EQUAL, "=");
  create_token(&gstate, T_INT, "16");
  create_token(&gstate, T_SEMICOLON, ";");

  create_token(&gstate, T_RIGHT_BRACE, "}");
  Ast_node *root = parse(&gstate);
  assert(get_binary(get_if(root).condition).op == binary_type::LESS);
  assert(darray_length(&get_block(get_if(root).if_block).statements, sizeof(Ast_node)) == 1);
  assert(get_if(root).else_block == NULL);
}

#define TEST_AST(fn, name)      \
  {                             \
    clear_token_state(&gstate); \
    TEST(fn, name)              \
  }

int main() {
  init_token_state(&gstate);
  TEST_AST(test_comparison, "comparison");
  TEST_AST(test_if, "if");
}
