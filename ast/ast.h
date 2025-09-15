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
    NODE_WHILE,
    NODE_IF,
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

typedef struct MethInfo{
    char* name;
    Args_List arguments;
    VarType returnType;
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
    MethInfo METH;
    ReturnInfo RETURN;
    NodeInfo NODE_INFO;
};

/*Estructuras para poder almacenar argumentos */
typedef struct Arg{
    char* name;
    VarType type;
    union values value;
}Arg;

typedef struct Args_List{
    Argument p;
    struct Args_List* next;
}Args_List;

/* Estructura de los nodos del ast */
typedef struct node{
    NodeType type;
    union type *info;
    struct node* left;
    struct node* right;
}node;

/* Constructores de nodos*/
node* create_int_node(int value);
node* create_bool_node(int value);
node* create_op_node(OpType name, VarType type);
node* create_id_node(char* name, VarType typeVar, NodeType type);
node* create_meth_node(char* name, Args_List arguments, VarType returnType);
node* create_return_node(VarType type);
node* create_node(char* info, VarType type);
node* new_node(NodeType type);
node* create_tree(node* root, node* left, node* right);

Arg new_arg(char* name, VarType type, int value);
void insert_arg(Args_List list, Argument);

/* Funciones para imprimir el arbol */
void print_node(node* root);
void print_tree(node* root, int level); 

#endif