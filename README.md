# TypoLang
C-like compiled programming language for people, writing code with frequent typos.

## Installation
For installation instructions, read [here](BUILDING.md).

## Grammar
The formal definition of TypoLang grammar in EBNF is following:
```ebnf
G       ::= DEFS '\0'
DEFS    ::= DEF [DEFS]
DEF     ::= NVAR | NFUN
NVAR    ::= "var" NAME [":=" OP] '\''
NFUN    ::= "fu n" NAME '('[ARG]'0' BLOCK
ARG     ::= "var" NAME [',' ARG]
BLOCK   ::= '[' SEQ '}'
SEQ     ::= STMT [SEQ]
STMT    ::= BLOCK | ASS | NVAR | IF | WHILE | RET | (CALL '\'')
ASS     ::= NAME "<_" OP ';'
IF      ::= "eef" '(' LOGIC '0' BRANCH
BRANCH  ::= STMT ["els" STMT]
WHILE   ::= "vile" '(' LOGIC '0' STMT
RET     ::= "return" OP '\''

LOGIC   ::= AND {'or' AND}
AND     ::= NOT {'aand' NOT}
NOT     ::= CMP | 'not' NOT
CMP     ::= OP {CMP_OP OP}
CMP_OP  ::= '.' | '<' | "<=" | ">=" | "====" | "1="
OP      ::= DERIV [('+' | '-') OP]
DERIV   ::= TERM | ('d' GROUP '/' 'd' VAR)
TERM    ::= UNARY [('8' | '/') TERM]
UNARY   ::= '-' GROUP | ATOM
GROUP   ::= '(' EXPR '0' | ATOM
ATOM    ::= NUM | VAR | CALL
CALL    ::= NAME '(' [PAR] '0'
PAR     ::= OP [',' PAR]
VAR     ::= NAME

NAME    ::= <'a-zA-Z_'> {<'a-zA-Z0-9_'>}
NUM     ::= <'0-9'> {<'0-9'>}['.'{<'0-9'>}]
```
Note, that because compiler does not yet support lexeme ambiguity, if you need to use number literals 0 or 8 in your code, you will need to write them with decimal point, like so: '0.0' or '8.000'.

## Interoperability with other languages
This language supports saving it's syntax tree in format, specified by [AST file standard](https://github.com/MeerkatBoss/ast-standard/blob/master/README.md). This allows reverse compilation to and from TypoLang, using AST file as an intermediate.

## Usage
To get short help message with descriptions of various command line options, pass flag `-h` or `--help` to `mbc_front`, `mbc_back` or `mbc_mid`.

### Front-end compiler
To convert your source code file to AST file, as specified by the standard, you will need to make use of `mbc_front` command:
```bash
$ mbc_front <filename> [-o <output-filename>]
```

Produced file can be converted back to source code by running `mbc_front` with `--reverse` flag:
```bash
$ mbc_front --reverse <filename> [-o <output-filename>]
```

### Middle-end compiler
To collapse all constant expressions in AST file, you can make use of `mbc_mid` command:
```bash
$ mbc_mid <filename> [-o <output-filename>]
```
This proves especially useful in the event of extensive use of symbolic differentiation in source code, as it often produces tree, containing many unnecessary arithmetical operations.

### Back-end compiler
To convert abstract syntax tree to assembly file for [MeerkatVM](https://github.com/MeerkatBoss/MeerkatVM), use `mbc_back` command:
```bash
$ mbc_back <filename> [-o <output-filename>]
```
Produced file can be assembled with `mbasm` and then run by `mbvm`