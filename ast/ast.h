// ast.h
#ifndef AST_H
#define AST_H

#include "../common.h"

/* Tipos de nodos del ast */
typedef enum{
    NODE_NUM,
    NODE_BOOL,
    NODE_OP,
    NODE_RET,
    NODE_DECL,
    NODE_ID_USE,
    NODE_METH,
    NODE_CALL_METH,
    NODE_IF_ELSE,
    NODE_WHILE,
    NODE_PYC,
    NODE_INFO
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
}node;

/*Estructuras para poder almacenar argumentos */
typedef struct Arg{
    char* name;
    VarType type;
    union values value;
}Arg;

typedef struct Args_List{
    Arg p;
    struct Args_List* next;
}Args_List;

/* Estructuras correspondientes a los distintos tipos de nodo */
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
    union values value;
}OpInfo;

typedef struct IdInfo{
    char* name;
    VarType type;
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

typedef struct MethInfo{
    char* name;
    Args_List* arguments;
    VarType returnType;
    int is_extern;
}MethInfo;

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
    MethInfo METH;
    ReturnInfo RETURN;
    NodeInfo NODE_INFO;
};

/* Constructores de nodos*/
node* create_int_node(int value);
node* create_bool_node(int value);
node* create_op_node(OpType name, VarType type);
node* create_id_node(char* name, VarType typeVar, NodeType type);
node* create_if_else_node(node* expr, node* if_block, node* else_block);
node* create_while_node(node* expr, node* block);
node* create_meth_node(char* name, Args_List* arguments, VarType returnType, NodeType type, int is_extern);
node* create_return_node(VarType type);
node* create_node(char* info, VarType type);
node* new_node(NodeType type);
node* create_tree(node* root, node* left, node* right);

Arg new_arg(char* name, VarType type, int value);
void insert_arg(Args_List** list, Arg a);

/* Funciones para imprimir el arbol */
void print_node(node* root, int level);
void print_tree(node* root, int level); 

/* Funciones auxiliares para imprimir el ast correctamente */
char* var_type_to_string(VarType type);
char* list_to_string(Args_List* args);

#endif