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

var_decl: type ID ASSIGN expr PYC
        ;

meth_decl: type ID PAREN_L meth_args PAREN_R block
         | type ID PAREN_L meth_args PAREN_R EXTERN PYC
         | VOID ID PAREN_L meth_args PAREN_R block
         | VOID ID PAREN_L meth_args PAREN_R EXTERN PYC
         ;

meth_args: 
         | args_list
         ;

args_list: type ID
         | args_list ',' type ID
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
         | meth_call PYC
         | IF PAREN_L expr PAREN_R THEN block if_else
         | WHILE expr block
         | RETURN expr PYC
         ;

if_else: 
       | ELSE block

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

type: BOOL
    | INT
    ;

meth_call: ID PAREN_L param_call_method PAREN_R
         ;

param_call_method: 
                 | param_list
                 ;

param_list: expr 
          | param_list ',' expr
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
