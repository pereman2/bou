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

* Lexer 
  - [ ] FA lexer?

* Code generation / IR representation
  - [ ] Improve instruction selection
  - [ ] Add instruction scheduler
  - [ ] Instruction allocation


Spec
-----

```
statement: block | ifstmt | loopstmt | funcstmt | structstmt 
block: { statement }
ifstmt:  if (expr) { statement }
        | if (expr) { statement } else {statement} 
funcstmt -> def ident(params): return_type block
return_type -> literal

; literal binary unary group

expr       -> decl 
decl       -> ident: literal = div
            | assignment
ident      -> [a-Z1-9]*
assignment -> ident = term
            | term
div       -> sum (('%'|'*')div)*
sum        -> literal (('+'|'-')sum)*
literal -> num 
        | float_num 
        | boolean 
        | char 
        | string

string -> "char*"
char -> duh

```

Example
-------
```
struct parser {
}

 def get_item(x: int, b: *parser, c: int): int {
    local: string = "xdfdsfd";
    i: int64 = null j: int64 = 0
    if (a == b) {
    } else {
    }
    for ( i: int = 2; i < local; i++) {
        i = i + 1
        j = 2 
    }
 }


def main():  {
    print(get_item())
}
```
