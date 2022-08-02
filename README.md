the BOU language
================


**todo**

* use FA lexer
* parser + ast
* IR optimizations
  - local
  - ...
* emit code https://github.com/skx/assembler/blob/master/compiler/compiler.go
* write an assembler https://github.com/vishen/go-x64-executable/blob/master/main.go
  (ELS executable)
  - instruction scheduler~~
  - register allocation


Spec
-----

statements -> block | ifstmt | loopstmt | funcstmt | structstmt 

literal binary unary group

expr       -> assignment 

decl       -> ident + colon + type + (equal + term)? 

assignment -> ident + equal + term

term       -> factor (('+'|'-')factor)*

factor     -> literal

literal -> num | float_num | boolean | char | string

string -> quote whatever quote


Example
-------
```
struct parser {
}

 def get_item(x: int, b: *parser, c: int): int {
    local: string = 2
    i: int64 = null j: int64 = 0
    for ( i: int = 2; i < local; i++) {
        i = i + 1
        j = 2 
    }
 }


def main():  {
    print(get_item())
}
```
