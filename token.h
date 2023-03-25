#pragma once
#include <cstring>
#include <string>
enum token_type {
  // TODO: add string literal, module, modulo, and, or, xor, neg ...
  T_INT,
  T_BOOL,
  T_STRING,
  T_STRING_LITERAL,
  T_CHAR,
  T_FLOAT,
  T_DOUBLE,

  // id of the type
  T_INTID,
  T_BOOLID,
  T_CHARID,
  T_FLOATID,
  T_DOUBLEID,
  T_STRINGID,

  T_DEF,
  T_STRUCT,
  T_IF,
  T_ELSE,
  T_FOR,
  T_WHILE,
  T_BREAK,
  T_CONTINUE,
  T_RETURN,
  T_IDENTIFIER,

  T_LEFT_PAR,
  T_RIGHT_PAR,
  T_LEFT_BRACE,
  T_RIGHT_BRACE,
  T_DOT,
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  T_SHIFT_RIGHT,
  T_SHIFT_LEFT,
  T_SEMICOLON,
  T_COLON,
  T_COMMA,
  T_AMPERSAND,

  T_BANG,
  T_BANG_EQUAL,
  T_EQUAL,
  T_EQUAL_EQUAL,
  T_GREATER,
  T_GREATER_EQUAL,
  T_LESS,
  T_LESS_EQUAL,

  T_OR,
  T_AND,

  T_BIT_OR,

  // Are this useful?
  T_TAB,
  T_LF,
  // this should be useful because:
  // v: int = 312432 4234
  // should fail
  // pasrser should have something like semicolon if we want more than one
  // expresion in the same line
  T_SPACE,

  T_ERROR,
  T_EOF,
};

struct Token {
  token_type type;
  char* start;
  char* end;
  int line;
};

inline void copy_token(Token* dst, Token* src) { memcpy(dst, src, sizeof(Token)); }
inline std::string token_to_str(Token* dst) {
  std::string s(dst->start, dst->end - dst->start);
  return s;
}
