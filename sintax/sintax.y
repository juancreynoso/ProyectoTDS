%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "ast.h"

    int yylex(void);
    int yyerror(const char *s);
    node* root;
    extern int yylineno;
%}
%code requires{
    #include "ast.h"
}

%union {
    int ival;
    char* sval;
    node* nd;
    Args_List* args;
    VarType vType;
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

%type <nd> declarations declaration var_decl meth_decl expr block meth_call statement statements if_else
%type <args> meth_args args_list param_call_method param_list
%type <vType> type

%%

prog: PROGRAM LLAVE_L declarations LLAVE_R {
        node* rt = create_node("Program", NONE);
        root = create_tree(rt, $3, NULL);
    }
    ;

declarations: { $$ = NULL; }
            | declarations declaration { 
                node* decl = create_node("decl", NONE);
                $$ = create_tree(decl, $2, $1);
            }
            ;

declaration: var_decl  {
                $$ = $1;
            }
            | meth_decl {
                $$ = $1;
            } 
            ;

var_decl: type ID ASSIGN expr PYC {
            node* id = create_id_node($2, $1, NODE_DECL);
            node* assign = create_op_node(OP_ASSIGN, NONE);
            $$ = create_tree(assign, id,$4);
        }
        ;

meth_decl: type ID PAREN_L meth_args PAREN_R block {
            node* meth = create_meth_node($2, $4, $1, NODE_METH, 0);
            $$ = create_tree(meth, $6, NULL);
         }  
         | type ID PAREN_L meth_args PAREN_R EXTERN PYC {
            $$ = create_meth_node($2, $4, $1, NODE_METH, 1);
         }
         | VOID ID PAREN_L meth_args PAREN_R block {
            node* meth = create_meth_node($2, $4, NONE, NODE_METH, 0);
            $$ = create_tree(meth, $6, NULL);
         }
         | VOID ID PAREN_L meth_args PAREN_R EXTERN PYC {
            $$ = create_meth_node($2, $4, NONE, NODE_METH, 1);
         }
         ;

meth_args: {$$ = NULL;}
         | args_list {
            $$ = $1;
         }
         ;

args_list: type ID {
            Arg p = new_arg($2, $1, 0);
            
            Args_List* list = NULL;
            insert_arg(&list, p);
            printf("%s",list_to_string(list));
            $$ = list;
         }
         | args_list ',' type ID {
            Arg p = new_arg($4, $3, 0);
            insert_arg(&$1, p);
            $$ = $1;
         }
         ;

block: LLAVE_L statements LLAVE_R {
        $$ = $2 ;
     }
     ;

statements: { $$ = NULL; }
          | statement statements {
            node* st = create_node("stmt", NONE);
            $$ = create_tree(st, $1, $2);
          }
          ;

statement: ID ASSIGN expr PYC {
            node* id = create_id_node($1, NONE, NODE_ID_USE);
            node* op = create_op_node(OP_ASSIGN, NONE);
            $$ = create_tree(op, id, $3);
         }
         | meth_call PYC {
            $$ = $1;
         }
         | IF PAREN_L expr PAREN_R THEN block if_else {
            $$ = create_if_else_node($3, $6, $7);
         }
         | WHILE expr block {
            $$ = create_while_node($2, $3);
         }
         | RETURN expr PYC {
            node* ret = create_node("ret", NONE);
            $$ = create_tree(ret, $2, NULL);
         }
         | PYC {$$ = new_node(NODE_PYC); }
         | block { $$ = $1; }
         ;

if_else: { $$ = NULL; }
       | ELSE block {
        $$ = $2;  
       }

expr: ID { $$ = create_id_node($1, NONE, NODE_ID_USE); }
    | NUM { $$ = create_int_node($1); }
    | meth_call {
        $$ = $1;
    }
    | expr PLUS expr {
        node* plus = create_op_node(OP_PLUS, TYPE_INT);
        $$ = create_tree(plus, $1, $3);
    }
    | expr GT expr {
        node* gt = create_op_node(OP_GT, TYPE_INT);
        $$ = create_tree(gt, $1, $3);
    }
    | expr LT expr {
        node* lt = create_op_node(OP_LT, TYPE_INT);
        $$ = create_tree(lt, $1, $3);
    }
    | expr SUB expr {
        node* sub = create_op_node(OP_SUB, TYPE_INT);
        $$ = create_tree(sub, $1, $3);
    }
    | expr MULT expr {
        node* mult = create_op_node(OP_MULT, TYPE_INT);
        $$ = create_tree(mult, $1, $3);
    }
    | expr DIV expr {
        node* div = create_op_node(OP_DIV, TYPE_INT);
        $$ = create_tree(div, $1, $3);
    }
    | expr REST expr {
        node* rest = create_op_node(OP_REST, TYPE_INT);
        $$ = create_tree(rest, $1, $3);
    }
    | expr EQUALS expr {
        node* equals = create_op_node(OP_EQUALS, NONE);
        $$ = create_tree(equals, $1, $3);
    }
    | PAREN_L expr PAREN_R { $$ = $2; }
    | SUB expr %prec MINUS {
        node* minus = create_op_node(OP_MINUS, TYPE_BOOL); 
        $$ = create_tree(minus, $2, NULL);
    }
    | VAL_BOOL { $$ = create_bool_node($1); }
    | NOT expr { 
        node* not = create_op_node(OP_NOT, TYPE_BOOL); 
        $$ = create_tree(not, $2, NULL);
    }
    | expr OR expr {
        node* or = create_op_node(OP_OR, TYPE_BOOL);
        $$ = create_tree(or, $1, $3);
    } 
    | expr AND expr {
        node* and = create_op_node(OP_AND, TYPE_BOOL);
        $$ = create_tree(and, $1, $3);
    }
    ;

meth_call: ID PAREN_L param_call_method  PAREN_R {
            $$ = create_meth_node($1 , NULL, NONE, NODE_CALL_METH, 0);
         }
         ;

param_call_method: { $$ = NULL; }
                 | param_list {
                    $$ = $1;
                 }
                 ;

param_list: expr { 
                
            }
          | param_list ',' expr {

          }
          ;

type: BOOL { $$ = TYPE_BOOL; }
    | INT { $$ = TYPE_INT; }
    ;
%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s near line %d\n", s, yylineno);
    return 0;
}
