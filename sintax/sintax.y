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
    Formal_P_List* f_params;
    Current_P_List* c_params;
    VarType vType;
}
%define parse.error verbose
%locations

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

%type <nd> declarations declaration var_decl var_decls meth_decl expr block meth_call statement statements if_else ret
%type <f_params> meth_args args_list 
%type <c_params> param_call_method param_list
%type <vType> type

%%

prog: PROGRAM LLAVE_L declarations LLAVE_R {
        node* rt = create_node("Program", NONE, @1.first_line, @1.first_column);
        root = create_tree(rt, $3, NULL);
    }
    ;

declarations: { $$ = NULL; }
            | declaration declarations { 
                node* decl = create_node("decl", NONE, @1.first_line, @1.first_column);
                $$ = create_tree(decl, $1, $2);
            }
            ;

declaration: var_decl  { $$ = $1; }
           | meth_decl { $$ = $1; } 
           ;

var_decl: type ID ASSIGN expr PYC {
            node* id = create_id_node($2, $1, NODE_DECL, @2.first_line, @2.first_column);
            node* assign = create_op_node(OP_ASSIGN, NONE, @3.first_line, @3.first_column);
            $$ = create_tree(assign, id, $4);
        }
        ;

meth_decl: type ID PAREN_L meth_args PAREN_R block {
            node* meth = create_meth_decl_node($2, $4, $1, 0, @2.first_line, @2.first_column);
            $$ = create_tree(meth, $6, NULL);
         }  
         | type ID PAREN_L meth_args PAREN_R EXTERN PYC {
            $$ = create_meth_decl_node($2, $4, $1, 1, @2.first_line, @2.first_column);
         }
         | VOID ID PAREN_L meth_args PAREN_R block {
            node* meth = create_meth_decl_node($2, $4, NONE, 0, @2.first_line, @2.first_column);
            $$ = create_tree(meth, $6, NULL);
         }
         | VOID ID PAREN_L meth_args PAREN_R EXTERN PYC {
            $$ = create_meth_decl_node($2, $4, NONE, 1, @2.first_line, @2.first_column);
         }
         ;

meth_args: {$$ = NULL;}
         | args_list { $$ = $1; }
         ;

args_list: type ID {
            Formal_P p = new_arg($2, $1, 0);
            Formal_P_List* list = NULL;
            insert_f_param(&list, p);
            $$ = list;
         }
         | args_list ',' type ID {
            Formal_P p = new_arg($4, $3, 0);
            insert_f_param(&$1, p);
            $$ = $1;
         }
         ;

block: LLAVE_L var_decls statements LLAVE_R {
        node* block = create_block_node("block", @1.first_line, @1.first_column);
        $$ = create_tree(block, $2, $3);
     }
     ;

var_decls: { $$ = NULL; }
         | var_decl var_decls {
            node* decl = create_node("decl", NONE, @1.first_line, @1.first_column);
            $$ = create_tree(decl, $1, $2);
         }
         ;

statements: { $$ = NULL; }
          | statement statements {
            node* st = create_node("stmt", NONE, @1.first_line, @1.first_column);
            $$ = create_tree(st, $1, $2);
          }
          ;

statement: ID ASSIGN expr PYC {
            node* id = create_id_node($1, NONE, NODE_ID_USE, @1.first_line, @1.first_column);
            node* op = create_op_node(OP_ASSIGN, NONE, @2.first_line, @2.first_column);
            $$ = create_tree(op, id, $3);
            }
          | meth_call PYC { $$ = $1; }
          | IF PAREN_L expr PAREN_R THEN block if_else {
                $$ = create_if_else_node($3, $6, $7, @1.first_line, @1.first_column);
            }
          | WHILE expr block {
                $$ = create_while_node($2, $3, @1.first_line, @1.first_column);
            }
          | ret { $$ = $1; }
          | PYC { $$ = new_node(NODE_PYC, @1.first_line, @1.first_column); }
          | block { $$ = $1; }
          ;

ret: RETURN PYC {
        node* ret = create_return_node(NONE, @1.first_line, @1.first_column);
        $$ = create_tree(ret, NULL, NULL);
    }
    | RETURN expr PYC {
        node* ret = create_return_node(NONE, @1.first_line, @1.first_column);
        $$ = create_tree(ret, $2, NULL);
    }
    ;

if_else: { $$ = NULL; }
       | ELSE block { $$ = $2; }
       ;

expr: ID { $$ = create_id_node($1, NONE, NODE_ID_USE, @1.first_line, @1.first_column); }
    | NUM { $$ = create_int_node($1, @1.first_line, @1.first_column); }
    | meth_call { $$ = $1; }
    | expr PLUS expr {
        node* plus = create_op_node(OP_PLUS, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(plus, $1, $3);
    }
    | expr GT expr {
        node* gt = create_op_node(OP_GT, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(gt, $1, $3);
    }
    | expr LT expr {
        node* lt = create_op_node(OP_LT, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(lt, $1, $3);
    }
    | expr SUB expr {
        node* sub = create_op_node(OP_SUB, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(sub, $1, $3);
    }
    | expr MULT expr {
        node* mult = create_op_node(OP_MULT, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(mult, $1, $3);
    }
    | expr DIV expr {
        node* div = create_op_node(OP_DIV, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(div, $1, $3);
    }
    | expr REST expr {
        node* rest = create_op_node(OP_REST, TYPE_INT, @2.first_line, @2.first_column);
        $$ = create_tree(rest, $1, $3);
    }
    | expr EQUALS expr {
        node* equals = create_op_node(OP_EQUALS, NONE, @2.first_line, @2.first_column);
        $$ = create_tree(equals, $1, $3);
    }
    | PAREN_L expr PAREN_R { $$ = $2; }
    | SUB expr %prec MINUS {
        node* minus = create_op_node(OP_MINUS, TYPE_BOOL, @1.first_line, @1.first_column); 
        $$ = create_tree(minus, $2, NULL);
    }
    | VAL_BOOL { $$ = create_bool_node($1, @1.first_line, @1.first_column); }
    | NOT expr { 
        node* not = create_op_node(OP_NOT, TYPE_BOOL, @1.first_line, @1.first_column); 
        $$ = create_tree(not, $2, NULL);
    }
    | expr OR expr {
        node* or = create_op_node(OP_OR, TYPE_BOOL, @2.first_line, @2.first_column);
        $$ = create_tree(or, $1, $3);
    } 
    | expr AND expr {
        node* and = create_op_node(OP_AND, TYPE_BOOL, @2.first_line, @2.first_column);
        $$ = create_tree(and, $1, $3);
    }
    ;

meth_call: ID PAREN_L param_call_method PAREN_R {
            $$ = create_meth_call_node($1, $3, @1.first_line, @1.first_column);
         }
         ;

param_call_method: { $$ = NULL; }
                 | param_list { $$ = $1; }
                 ;

param_list: expr { 
                Current_P_List* list = NULL;
                insert_c_param(&list, $1);  
                $$ = list;
            }
          | param_list ',' expr {
                insert_c_param(&$1, $3);  
                $$ = $1;
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
