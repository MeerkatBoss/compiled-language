```ebnf
G       ::= DEFS '\0'
DEFS    ::= DEF [DEFS]
DEF     ::= NVAR | NFUN
NVAR    ::= "var" NAME ["=" EXPR] ';'
NFUN    ::= "fun" NAME (ARG) BLOCK
ARG     ::= ["var" NAME ARG]
BLOCK   ::= ['{' {STMT} '}']
STMT    ::= BLOCK | ASS | NVAR | IF | WHILE | RET
EXPR    ::= OR
ASS     ::= NAME "<-" EXPR ';'
IF      ::= "if" '(' EXPR ')' BRANCH
BRANCH  ::= STMT ["else" STMT]
WHILE   ::= "while" '(' EXPR ')' STMT
RET     ::= "return" EXPR ';'

OR      ::= AND ['||' OR]
AND     ::= CMP ['&&' AND]
CMP     ::= SUM [CMP_OP SUM]
CMP_OP  ::= '>' | '<' | "<=" | ">=" | "==" | "!="
SUM     ::= TERM [('+' | '-') SUM]
TERM    ::= UNARY [('*' | '/') TERM]
UNARY   ::= '-' UNARY | ATOM
ATOM    ::= NUM | VAR | CALL
CALL    ::= NAME '(' PAR ')'
PAR     ::= [EXPR ',' PAR]
VAR     ::= NAME

NAME    ::= <'a-zA-Z0-9_'> {<'a-zA-Z0-9_'>}
NUM     ::= <'0-9'> {<'0-9'>}
```