%{
    #include <stdlib.h>
    #include <stdio.h>

    int yylex(void);
    int yyerror(const char *s);

    extern int yylineno;
%}

%token PROGRAM EXTERN VOID BOOL INT RETURN CONST IF ELSE THEN WHILE
%token EQUALS ASSIGN PLUS SUB MULT DIV REST LT GT OR AND NOT 
%token PAREN_L PAREN_R LLAVE_L LLAVE_R
%token COMEN_OLINE COMEN_MLINE_L COMEN_MLINE_R
%token VAL_BOOL NUM ID 
%token PYC

%left PLUS SUB
%left MULT DIV REST
%left AND OR
%right NOT  

%%

prog: PROGRAM LLAVE_L var_decl_list LLAVE_R
    ;

var_decl_list: 
             | var_decl var_decl_list
             ;

var_decl: type ID ASSIGN expr PYC
        ;

expr: ID
    | NUM
    | expr PLUS expr
    | expr SUB expr
    | expr MULT expr
    | expr DIV expr
    | expr REST expr
    | PAREN_L expr PAREN_R
    | VAL_BOOL
    | NOT expr
    | expr OR expr
    | expr AND expr
    ;

type: BOOL
    | INT
    ;


%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s near line %d\n", s, yylineno);
    return 0;
}

int main() {
    if (yyparse() == 0) {
        printf("Parseado correctamente, sin errores.\n");
    }
    return 0;
}
