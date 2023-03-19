#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "ast_node.h"
#include "darray.h"
#include "debug.h"
#include "token.h"

// Identifier of the ast_node
int ast_num = 0;

void print_ast(Ast_node *root) {
  eval_ast(root);
  auto representation = ast_node_to_string(root);
  std::cout << representation << std::endl;
}

// add identifiers to each ast_node
void eval_ast(Ast_node *root) {}

std::string ast_node_to_string(Ast_node *root) {
  std::stringstream ss;
  switch (root->type) {
    case EXPRESSION:
      return ast_expression_to_string(root);
    case STATEMENT:
      return ast_statement_to_string(root);
  }
  return ss.str();
}

std::string ast_expression_to_string(Ast_node *root) {
  std::stringstream ss;
  switch(root->value.expression.type) {
    case BINARY:
      ss << root << " BINARY (" << ast_node_to_string(get_binary(root).left)
        << " + " << ast_node_to_string(get_binary(root).right) << ")";
      break;
    case LITERAL:
      ss << root << " LITERAL " << literal_repr(&get_literal(root));
      break;
    case IDENTIFIER:
      std::string name;
      char *start = get_identifier(root).token->start;
      char *end = get_identifier(root).token->end;
      while (start < end) {
        name.push_back(*start);
        start++;
      }
      std::string type(get_identifier(root).type->start,
          get_identifier(root).type->end -
          get_identifier(root).type->start);
      ss << root << " IDENT " << type << " " << name;
      break;
  }
  return ss.str();
}

std::string ast_statement_to_string(Ast_node *root) {
  std::stringstream ss;
  switch(root->value.statement.type) {
    case BLOCK: {
                  ss << ast_block_to_str(&get_block(root), "");
                  break;
                }
    case IF: {
                  ss << "if (\n";
                  ss << ast_node_to_string(get_if(root).condition);
                  ss << "\n){\n";
                  ss << ast_block_to_str(&get_block(get_if(root).if_block), "");
                  if (get_if(root).else_block != NULL) {
                    ss << "\nelse {\n";
                    ss << ast_block_to_str(&get_block(get_if(root).else_block), "");
                  }
                  break;
                }
    case FUNC: {
                 AstFunc* func = &get_func(root);
                 ss << "params:\n";
                 for (int i = 0; i < darray_length(&func->parameters, sizeof(AstParameter)); i++) {
                   AstParameter* param = (AstParameter*)darray_get(&func->parameters, sizeof(AstParameter), i);
                   ss << token_to_str(&param->token) << ": " << token_to_str(&param->type) << "\n";
                 }
                 ss << ast_block_to_str(&get_block(func->block), func->name);
                 break;
               }
  }
  return ss.str();
}

std::string ast_block_to_str(AstBlock *block, std::string block_name) {
  std::stringstream ss;
  ss << "@" << block_name << "{\n";
  for (int i = 0;
      i < darray_length(&block->statements, sizeof(Ast_node));
      i++) {
    Ast_node* node = (Ast_node*)darray_get(&block->statements, sizeof(Ast_node), i);
    ss << ast_node_to_string(node);
    ss << "\n";
  }
  ss << "}\n";
  return ss.str();
}

std::string literal_repr(AstLiteral *literal) {
  switch (literal->type) {
  case AstLiteral::INT:
    return std::to_string(literal->value.i);
  case AstLiteral::CHAR:
    return std::to_string(literal->value.c);
  case AstLiteral::FLOAT:
    return std::to_string(literal->value.f);
  case AstLiteral::BOOL:
    return std::to_string(literal->value.b);
  default:
    return "debug: unknown literal type\n";
  }
}

void print_tokens(Token **tokens, int len) {
  for (int i = 0; i < len; i++) {
    print_token(tokens[i]);
  }
}

void print_token(Token *t) {
  const char *type;
  switch (t->type) {
  case T_INT:
    type = "T_INT";
    break;
  case T_BOOL:
    type = "T_BOOL";
    break;
  case T_INTID:
    type = "T_INTID";
    break;
  case T_BOOLID:
    type = "T_BOOLID";
    break;
  case T_BREAK:
    type = "T_BREAK";
    break;
  case T_RETURN:
    type = "T_RETURN";
    break;
  case T_CONTINUE:
    type = "T_CONTINUE";
    break;
  case T_STRING:
    type = "T_STRING";
    break;
  case T_STRING_LITERAL:
    type = "T_STRING_LITERAL";
    break;
  case T_CHAR:
    type = "T_CHAR";
    break;
  case T_CHARID:
    type = "T_CHARID";
    break;
  case T_DEF:
    type = "T_DEF";
    break;
  case T_STRUCT:
    type = "T_STRUCT";
    break;
  case T_IF:
    type = "T_IF";
    break;
  case T_ELSE:
    type = "T_ELSE";
    break;
  case T_WHILE:
    type = "T_WHILE";
    break;
  case T_FLOAT:
    type = "T_FLOAT";
    break;
  case T_DOUBLE:
    type = "T_DOUBLE";
    break;
  case T_FLOATID:
    type = "T_FLOATID";
    break;
  case T_DOUBLEID:
    type = "T_DOUBLEID";
    break;
  case T_IDENTIFIER:
    type = "T_IDENTIFIER";
    break;
  case T_LEFT_PAR:
    type = "T_LEFT_PAR";
    break;
  case T_RIGHT_PAR:
    type = "T_RIGHT_PAR";
    break;
  case T_LEFT_BRACE:
    type = "T_LEFT_BRACE";
    break;
  case T_RIGHT_BRACE:
    type = "T_RIGHT_BRACE";
    break;
  case T_DOT:
    type = "T_DOT";
    break;
  case T_PLUS:
    type = "T_PLUS";
    break;
  case T_MINUS:
    type = "T_MINUS";
    break;
  case T_STAR:
    type = "T_STAR";
    break;
  case T_SLASH:
    type = "T_SLASH";
    break;
  case T_SHIFT_RIGHT:
    type = "T_SHIFT_RIGHT";
    break;
  case T_SHIFT_LEFT:
    type = "T_SHIFT_LEFT";
    break;
  case T_SEMICOLON:
    type = "T_SEMICOLON";
    break;
  case T_COLON:
    type = "T_COLON";
    break;
  case T_COMMA:
    type = "T_COMMA";
    break;
  case T_BANG:
    type = "T_BANG";
    break;
  case T_BANG_EQUAL:
    type = "T_BANG_EQUAL";
    break;
  case T_EQUAL:
    type = "T_EQUAL";
    break;
  case T_EQUAL_EQUAL:
    type = "T_EQUAL_EQUAL";
    break;
  case T_GREATER:
    type = "T_GREATER";
    break;
  case T_GREATER_EQUAL:
    type = "T_GREATER_EQUAL";
    break;
  case T_LESS:
    type = "T_LESS";
    break;
  case T_LESS_EQUAL:
    type = "T_LESS_EQUAL";
    break;
  case T_TAB:
    type = "T_TAB";
    break;
  case T_LF:
    type = "T_LF";
    break;
  case T_SPACE:
    type = "T_SPACE";
    break;
  case T_ERROR:
    type = "T_ERROR";
    break;
  case T_EOF:
    type = "T_EOF";
    break;
  default:
    type = "Uknown token";
    break;
  }
  printf("Token %s - stored value \"%.*s\" - line %d - mem %p\n", type,
         (int)(t->end - t->start), t->start, t->line, t->start);
}

void dump_darray(darray *da) {
  for (int i = 0; i < da->count; i++) {
    printf("0x%02X ", (unsigned char)da->src[i]);
    // let's write 8 bytes per line
    if (i % 8 == 0 && i > 0) {
      printf("\n");
    }
  }
}
