#pragma once

#include "token.h"
#include <vector>

struct Scanner {
  char *ip, *start;
  int line;
  Token **tokens;
  int token_capacity;
  int token_count;
};

int get_tokens_len();
void init_scanner(char *src);
char next_char();
bool match(const char *to_match, int len);
Token *create_token(token_type type);
Token *scan_token();
Token **scan(char *src);
