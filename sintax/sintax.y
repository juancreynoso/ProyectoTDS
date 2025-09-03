%{
    #include <stdlib.h>
    #include <stdio.h>

    int yyerror(const char *s);
%}

%token PROGRAM EXTERN VOID BOOL INT RETURN CONST IF ELSE THEN WHILE
%token EQUALS ASSIGN PLUS SUB MULT DIV MOD LT GT OR AND NEG 
%token PAREN_L PAREN_R LLAVE_L LLAVE_R
%token COMEN_OLINE COMEN_MLINE_L COMEN_MLINE_R
%token VAL_BOOL NUM ID 
%token PYC

%%

prog: PROGRAM LLAVE_L LLAVE_R
    ;



%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s near line %d\n", s, yylineno);
    return 0;
}

int main() {
    if (yyparse() == 0) {
        printf("Parseado correctamente, sin errores.\n");
        printInOrder(root);
        printTree(root, 0); 
    }
    return 0;
}
