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


expr -> binary 
term -> factor (('+'|'-')factor)*
factor -> literal
literal -> num, float_num, boolean, char, string


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
