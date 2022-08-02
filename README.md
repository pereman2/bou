the BOU language
================


Why
---

bored

Current functionality:
---------------------

sum of 2 ints :O

```sh
cmake .
make
echo "3+3" | ./bou
./program
```

outstanding optimizations:
```asm
0000000080000078 <.text>:
    80000078:   b8 03 00 00 00          mov    $0x3,%eax
    8000007d:   b9 03 00 00 00          mov    $0x3,%ecx
    80000082:   01 c8                   add    %ecx,%eax
    80000084:   b8 01 00 00 00          mov    $0x1,%eax
    80000089:   bb 00 00 00 00          mov    $0x0,%ebx
    8000008e:   cd 80                   int    $0x80
```
Todo
----

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
