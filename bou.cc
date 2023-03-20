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
  if (args > 1) {
    char *buffer = 0;
    int length;
    FILE *f = fopen (argv[1], "r"); 
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = (char*)malloc ((length+1)*sizeof(char));
    if (buffer)
    {
      fread (buffer, sizeof(char), length, f);
    }
    fclose (f);
    compile(buffer);
  } else {
    repl();
  }
  return 0;
}
