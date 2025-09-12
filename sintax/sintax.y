%{
    #include <stdlib.h>
    #include <stdio.h>

    int yylex(void);
    int yyerror(const char *s);
    node* root;
    extern int yylineno;
%}

%union {
    int ival;
    char* sval;
    node* nd;
}

%token PROGRAM EXTERN VOID BOOL INT RETURN CONST IF ELSE THEN WHILE
%token EQUALS ASSIGN PLUS SUB MULT DIV REST LT GT OR AND NOT
%token PAREN_L PAREN_R LLAVE_L LLAVE_R
%token <ival> VAL_BOOL NUM 
%token <sval> ID 
%token PYC

%left AND OR
%left EQUALS
%left LT GT
%left PLUS SUB
%left MULT DIV REST
%right NOT
%right MINUS

%type <nd> declarations declaration var_decl meth_decl

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
         | PYC
         | block
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
    | SUB expr %prec MINUS
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
