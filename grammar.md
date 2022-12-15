```ebnf
G       ::= DEFS '\0'
DEFS    ::= DEF [DEFS]
DEF     ::= NVAR | NFUN
NVAR    ::= "var" NAME ["=" OP] ';'
NFUN    ::= "fun" NAME (ARG) BLOCK
ARG     ::= ["var" NAME ARG]
BLOCK   ::= ['{' {STMT} '}']
STMT    ::= BLOCK | ASS | NVAR | IF | WHILE | RET
ASS     ::= NAME "<-" OP ';'
IF      ::= "if" '(' LOGIC ')' BRANCH
BRANCH  ::= STMT ["else" STMT]
WHILE   ::= "while" '(' LOGIC ')' STMT
RET     ::= "return" OP ';'

LOGIC   ::= OR
OR      ::= AND {'||' AND}
AND     ::= NOT {'&&' NOT}
NOT     ::= CMP | '!' NOT
CMP     ::= SUM CMP_OP SUM
CMP_OP  ::= '>' | '<' | "<=" | ">=" | "==" | "!="
SUM     ::= TERM [('+' | '-') SUM]
TERM    ::= UNARY [('*' | '/') TERM]
UNARY   ::= '-' UNARY | ATOM
ATOM    ::= NUM | VAR | CALL
CALL    ::= NAME '(' PAR ')'
PAR     ::= [OP ',' PAR]
VAR     ::= NAME

NAME    ::= <'a-zA-Z0-9_'> {<'a-zA-Z0-9_'>}
NUM     ::= <'0-9'> {<'0-9'>}
```