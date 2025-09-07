%{
    #include <stdlib.h>
    #include <stdio.h>

    int yylex(void);
    int yyerror(const char *s);

    extern int yylineno;
%}

%token PROGRAM EXTERN VOID BOOL INT RETURN CONST IF ELSE THEN WHILE
%token EQUALS ASSIGN PLUS SUB MULT DIV REST LT GT OR AND NOT 
%token PAREN_L PAREN_R LLAVE_L LLAVE_R COMA
%token COMEN_OLINE COMEN_MLINE_L COMEN_MLINE_R
%token VAL_BOOL NUM ID 
%token PYC

%left PLUS SUB
%left EQUALS
%left MULT DIV REST
%left LT GT
%left AND OR
%right NOT


%%

prog: PROGRAM LLAVE_L declarations LLAVE_R
    ;

declarations:
            | declarations declaration
            ;

declaration: var_decl 
           | meth_decl
           ;

var_decl: var_type ID ASSIGN expr PYC
        ;

meth_decl: var_type ID PAREN_L meth_args PAREN_R block
         | var_type ID PAREN_L meth_args PAREN_R EXTERN PYC
         | VOID ID PAREN_L meth_args PAREN_R block
         | VOID ID PAREN_L meth_args PAREN_R EXTERN PYC
         ;

meth_args: 
         | args_list
         ;

args_list: var_type ID
         | args_list COMA var_type ID
         ; 

block: LLAVE_L var_decls statements LLAVE_R
     ;

var_decls: 
         | var_decl var_decls
         ;

statements: 
          | statement statements
          ;

statement: ID ASSIGN expr PYC
         | meth_call
         | IF PAREN_L expr PAREN_R 
         | RETURN expr PYC
         ;

expr: ID
    | NUM
    | meth_call
    | expr PLUS expr
    | expr GT expr
    | expr LT expr
    | expr SUB expr
    | expr MULT expr
    | expr DIV expr
    | expr REST expr
    | expr EQUALS expr 
    | PAREN_L expr PAREN_R
    | SUB expr
    | VAL_BOOL
    | NOT expr
    | expr OR expr
    | expr AND expr
    ;

var_type: BOOL
        | INT
        ;

meth_call: ID PAREN_L param_call_method PAREN_R PYC
         ;

param_call_method: 
                 | param_list
                 ;

param_list: expr param_list_tail
          ;

param_list_tail: 
               | COMA expr param_list_tail
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

/*
block: LLAVE_L var_decl statements LLAVE_R
     ;

statements: statement statements
          ;

statement: ID ASSIGN expr PYC
         | method_call PYC
         | IF PAREN_L expr PAREN_R THEN block ELSE block
*/