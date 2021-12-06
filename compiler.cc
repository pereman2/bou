#include "debug.h"
#include "scanner.h"
#include "token.h"


void compile(char *src) {
  Token **tokens = scan(src);
  int len_tokens = get_tokens_len();
  print_tokens(tokens, len_tokens);
}
