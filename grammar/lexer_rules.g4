lexer grammar lexer_rules;

WS: [ \t\r\n]+ -> skip;

ID: [a-zA-Z]+;
INT: '-'? [0-9]+;
FLOAT: '-'? [0-9]+ '.' [0-9]+ 'F';

BINOP: '+' | '-' | '*' | '/';
UNOP: '-';