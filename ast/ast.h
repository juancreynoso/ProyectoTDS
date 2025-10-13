// ast.h
#ifndef AST_H
#define AST_H

#include <stdio.h>  
#include "../common.h"
#include "param_list.h"

/* Tipos de nodos del ast */
typedef enum{
    NODE_NUM,
    NODE_BOOL,
    NODE_OP,
    NODE_RET,
    NODE_DECL,
    NODE_ID_USE,
    NODE_DECL_METH,
    NODE_CALL_METH,
    NODE_IF_ELSE,
    NODE_WHILE,
    NODE_PYC,
    NODE_INFO,
    NODE_BLOCK
}NodeType;

/* Tipos de operaciones */
typedef enum{
    OP_ASSIGN,
    OP_PLUS,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    OP_REST,
    OP_MINUS,
    OP_GT,
    OP_LT,
    OP_EQUALS,
    OP_AND,
    OP_OR,
    OP_NOT
}OpType;

union values{
    int num;
    int boolean;
};

/* Estructura de los nodos del ast */
typedef struct node{
    NodeType type;
    union type *info;
    struct node* left;
    struct node* right;
    int line;
    int column;
} node;

/* ---- Estructuras correspondientes a los distintos tipos de nodo ---- */

typedef struct  IntInfo{
    int value;
    VarType type;
}IntInfo;

typedef struct  BoolInfo{
    int value;
    VarType type;
}BoolInfo;

typedef struct OpInfo{
    OpType name;
    VarType type;
    int is_glbl;
    union values value;
}OpInfo;

typedef struct IdInfo{
    char* name;
    VarType type;
    int is_glbl;
    union values value;
}IdInfo;

typedef struct If_Else_Info{
    node* expr;
    node* if_block;
    node* else_block;
}If_Else_Info;

typedef struct WhileInfo{
    node* expr;
    node* block;
}WhileInfo;

typedef struct MethDecl_Info{
    char* name;
    Formal_P_List* f_params;
    VarType returnType;
    int is_extern;
}MethDecl_Info;

typedef struct MethCall_Info{
    char* name;
    Formal_P_List* f_params;
    Current_P_List* c_params;
    VarType returnType;
}MethCall_Info;

typedef struct BlockInfo {
    char* name;
} BlockInfo;

typedef struct ReturnInfo{
    VarType type;
    union values value;
}ReturnInfo;

typedef struct NodeInfo{
    char* info;
    VarType type;
}NodeInfo;

/* Info que llevan los nodos*/
union type{
    IntInfo INT;
    BoolInfo BOOL;
    OpInfo OP;
    IdInfo ID;
    If_Else_Info IF_ELSE;
    WhileInfo WHILE;
    MethDecl_Info METH_DECL;
    MethCall_Info METH_CALL;
    ReturnInfo RETURN;
    NodeInfo NODE_INFO;
    BlockInfo BLOCK_INFO;
};


/* ---- Constructores de nodos ---- */
node* create_int_node(int value, int line, int column);
node* create_bool_node(int value, int line, int column);
node* create_op_node(OpType name, VarType type, int line, int column);
node* create_id_node(char* name, VarType typeVar, NodeType type, int line, int column);
node* create_if_else_node(node* expr, node* if_block, node* else_block, int line, int column);
node* create_while_node(node* expr, node* block, int line, int column);
node* create_meth_decl_node(char* name, Formal_P_List* f_params, VarType returnType, int is_extern, int line, int column);
node* create_meth_call_node(char*name, Current_P_List* c_params, int line, int column);
node* create_block_node(char* name, int line, int column);
node* create_return_node(VarType type, int line, int column);
node* create_node(char* info, VarType type, int line, int column);
node* new_node(NodeType type, int line, int column);
node* create_tree(node* root, node* left, node* right);

/* -- Funciones para almacenar el arbol en un archivo de salida -- */
void print_node(node* root, int level, FILE* parser_out);
void save_ast(node* root, int level, FILE* parser_out); 

#endif