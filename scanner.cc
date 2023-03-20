#include "scanner.h"
#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <vector>

Scanner scanner;

void init_scanner(char *src) {
  scanner.ip = src;
  scanner.start = src;
  scanner.line = 1;

  scanner.token_count = 0;
  scanner.token_capacity = 8;
  scanner.tokens = (Token **)malloc(sizeof(Token *) * scanner.token_capacity);
  ensure_alloc(scanner.tokens, __FILE__, __LINE__);
}

void free_scanner() {
  while (scanner.token_count--) {
    free(scanner.tokens[scanner.token_count]);
  }
  scanner.token_capacity = 8;
  free(scanner.tokens);
}

char peek() { return *scanner.ip; }

char next_char() { return *(scanner.ip++); }

bool is_numeric(char c) { return c >= '1' && c <= '9'; }

bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool match(const char *to_match, size_t len, size_t size) {
  return len == size && memcmp(to_match, scanner.ip, size) == 0;
}

Token *create_token(token_type type, int advance) {
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->line = scanner.line;
  token->start = scanner.start;

  scanner.ip += advance;
  token->end = scanner.ip;
  return token;
}

Token *create_numeric_token() {
  int len = 0;
  token_type type = T_INT;
  while (is_numeric(*(scanner.ip + len))) {
    len++;
  }
  if (*(scanner.ip + len) == '.') {
    len++;
    while (is_numeric(*(scanner.ip + len))) {
      len++;
    }
    type = T_FLOAT;
  }
  return create_token(type, len);
}

Token *create_identifier_token() {
  int len = 0;
  char c = next_char();
  /* get whole word */
  while (is_alpha(*(scanner.ip + len))) {
    len++;
  }

  switch (c) {
  case 'i':
    if (match("nt", 2, len)) {
      return create_token(T_INTID, 2);
    }
    if (match("f", 1, len)) {
      return create_token(T_IF, 1);
    }
    break;
  case 'b':
    if (match("ool", 3, len)) {
      return create_token(T_BOOLID, 3);
    }
    if (match("reak", 4, len)) {
      return create_token(T_BREAK, 4);
    }
    break;
  case 'c':
    if (match("har", 3, len)) {
      return create_token(T_CHARID, 0);
    }
    if (match("ontinue", 3, len)) {
      return create_token(T_CONTINUE, 0);
    }
    break;
  case 'd':
    if (match("ef", 2, len)) {
      return create_token(T_DEF, 2);
    }
    if (match("ouble", 5, len)) {
      return create_token(T_DOUBLEID, 5);
    }
    break;
  case 'e':
    if (match("lse", 3, len)) {
      return create_token(T_ELSE, 3);
    }
    break;
  case 'f':
    if (match("or", 3, len)) {
      return create_token(T_FOR, 3);
    }
    if (match("loat", 4, len)) {
      return create_token(T_FLOATID, 4);
    }
    if (match("alse", 4, len)) {
      return create_token(T_BOOL, 4);
    }
    break;
  case 'r':
    if (match("eturn", 5, len)) {
      return create_token(T_RETURN, 5);
    }
    break;
  case 's':
    if (match("tring", 5, len)) {
      return create_token(T_STRINGID, 5);
    }
    if (match("truct", 5, len)) {
      return create_token(T_STRUCT, 5);
    }
    break;
  case 't':
    if (match("rue", 3, len)) {
      return create_token(T_BOOL, 3);
    }
    break;
  case 'w':
    if (match("hile", 4, len)) {
      return create_token(T_WHILE, 4);
    }
    break;
  }
  // identifier can include numeric infixes and suffixes
  while (is_alpha(*(scanner.ip + len)) || is_numeric(*(scanner.ip + len))) {
    len++;
  }
  return create_token(T_IDENTIFIER, len);
}

Token *create_string_token() {
  int len = 0;
  char c;
  do {
    c = *(scanner.ip + len++);
  } while (c != '"' && c != ' ' && c != 0 && c != '\n');

  if (c == '"') {
    return create_token(T_STRING_LITERAL, len);
  }
  return create_token(T_ERROR, len);
}

// TODO: change to use a FA(finite automata)
// this could be done by a generating script where it creates
// the whole automata structure.
// references:
// - 4coder lexer generator https://youtu.be/Jw9kJKuh-RU?t=2189
// - strategies for fast lexical analysis
//   paper: https://nothings.org/computer/lexing.html
//   src: https://github.com/nothings/stb/blob/master/stb_c_lexer.h
// - cool tutorial creating interpreters:
//   https://github.com/lotabout/write-a-C-interpreter/blob/master/tutorial/en/3-Lexer.md
// - lexer struct
//   https://youtu.be/HxaD_trXwRE?t=1034
// - go lexer with concurrency
//   https://www.youtube.com/watch?v=HxaD_trXwRE

int get_tokens_len() { return scanner.token_count; }

void append_token(Token *token) {
  if (scanner.token_count >= scanner.token_capacity) {
    scanner.token_capacity *= 2;
    scanner.tokens = (Token **)realloc(
        scanner.tokens, sizeof(Token *) * scanner.token_capacity);
    ensure_alloc(scanner.tokens, __FILE__, __LINE__);
  }
  scanner.tokens[scanner.token_count++] = token;
}

Token **scan(char *src) {
  init_scanner(src);
  Token *token;
  do {
    token = scan_token();
    append_token(token);
  } while (token->type != T_EOF && token->type != T_ERROR);
  return scanner.tokens;
}

void ignore() {
  while (*scanner.ip == ' ') {
    scanner.ip++;
  }
}
Token *scan_token() {
  ignore();
  scanner.start = scanner.ip;
  if (is_numeric(peek())) {
    return create_numeric_token();
  }

  if (is_alpha(peek())) {
    return create_identifier_token();
  }

  // operators, cosmetic and such
  char c = next_char();
  switch (c) {
  case '"':
    return create_string_token();
  case '(':
    return create_token(T_LEFT_PAR, 0);
    break;
  case ')':
    return create_token(T_RIGHT_PAR, 0);
    break;
  case '{':
    return create_token(T_LEFT_BRACE, 0);
    break;
  case '}':
    return create_token(T_RIGHT_BRACE, 0);
    break;
  case '.':
    return create_token(T_DOT, 0);
    break;
  case '+':
    return create_token(T_PLUS, 0);
    break;
  case '-':
    return create_token(T_MINUS, 0);
    break;
  case '*':
    return create_token(T_STAR, 0);
    break;
  case '/':
    return create_token(T_SLASH, 0);
    break;
  case ';':
    return create_token(T_SEMICOLON, 0);
    break;
  case ':':
    return create_token(T_COLON, 0);
    break;
  case ',':
    return create_token(T_COMMA, 0);
    break;
  case '!':
    if (match("=", 1, 1)) {
      return create_token(T_BANG_EQUAL, 1);
    }
    return create_token(T_BANG, 0);
  case '=':
    if (match("=", 1, 1)) {
      return create_token(T_EQUAL_EQUAL, 1);
    }
    return create_token(T_EQUAL, 0);
  case '>':
    if (match("=", 1, 1)) {
      return create_token(T_GREATER_EQUAL, 1);
    }
    if (match(">", 1, 1)) {
      return create_token(T_SHIFT_RIGHT, 1);
    }
    return create_token(T_GREATER, 0);
  case '<':
    if (match("=", 1, 1)) {
      return create_token(T_LESS_EQUAL, 1);
    }
    if (match("<", 1, 1)) {
      return create_token(T_SHIFT_LEFT, 1);
    }
    return create_token(T_LESS, 0);
  case '\t':
    return create_token(T_TAB, 0);
    break;
  case '\n':
    scanner.line++;
    return create_token(T_LF, 0);
    break;
  case ' ':
    return create_token(T_SPACE, 0);
  case EOF:
    return create_token(T_EOF, 0);
  default:
    break;
  }
  return create_token(T_ERROR, 0);
}
