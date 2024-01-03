grammar Kuso;

import lexer_rules;

program:
    statement+
;

statement:
    assignment
    | expression
;

assignment:
    expression '->' ID
;

expression:
    ID
    | INT
    | FLOAT
    | expression BINOP expression
    | '(' expression ')'
    | UNOP expression
;