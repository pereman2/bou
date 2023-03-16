#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

void repl() {

  char line[1024];
  while (1) {
    fgets(line, sizeof(line)-1, stdin);
    char *eof = line;
    while(*eof != '\0'){eof++;}
    *eof = EOF;
    compile(line);
  }
}
int main(int args, char** argv) {
  repl();
  return 0;
}
