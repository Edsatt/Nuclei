> The aim of this project was to make a parser and interpreter for a simple language (see bottom of this file).
> general.c, general.h and the Makefile were provided

# (N)eill's (U)n(C)ommon (L)isp (E)xpression (I)nterpreter
<PROG>     ::= "(" <INSTRCTS>

<INSTRCTS> ::= <INSTRCT> <INSTRCTS> | ")"

<INSTRCT>  ::= "(" <FUNC> ")"

<FUNC>     ::= <RETFUNC> | <IOFUNC> | <IF> | <LOOP>

<RETFUNC>  ::= <LISTFUNC> | <INTFUNC> | <BOOLFUNC>

<LISTFUNC> ::= "CAR" <LIST> | "CDR" <LIST> | "CONS" <LIST> <LIST>

<INTFUNC>  ::= "PLUS" <LIST> <LIST> | "LENGTH" <LIST>

<BOOLFUNC> ::= "LESS" <LIST> <LIST> | "GREATER" <LIST> <LIST> | "EQUAL" <LIST> <LIST>

<IOFUNC>   ::= <SET> | <PRINT>
<SET>      ::= "SET" <VAR> <LIST>
<PRINT>    ::= "PRINT" <LIST> | "PRINT" <STRING>

# (IF (CONDITION) ((IF-TRUE-INSTRCTS)) ((IF_FALSE_INSTRCTS)))
<IF>       ::= "IF" "(" <BOOLFUNC> ")" "(" <INSTRCTS> "(" <INSTRCTS>

<LOOP>     ::= "WHILE""(" <BOOLFUNC> ")" "(" <INSTRCTS>

<LIST>     ::= <VAR> | <LITERAL> | "NIL" | "(" <RETFUNC> ")"

# Variables are just one letter A, B, Z etc.
<VAR>      ::= [A-Z]

# For printing only:
<STRING>   ::= Double-quoted string constant e.g. "Hello, World!", or "FAILURE ?"

# Since lists appear in bracket, to differentiate them
# from instructions, we use 'single' quotes:
# For the parser, anything in single-quotes is fine - you don't need to "investigate" whether it's valid or not.
<LITERAL> ::= Single-quoted list e.g. '(1)', '(1 2 3 (4 5))', or '2'
