// param_list
#include <stdio.h>  

#ifndef PARAM_LIST
#define PARAM_LIST

#include "../common.h"
typedef struct node node;

/* Estructura para los parametros formales */
typedef struct Formal_P{
    char* name;
    VarType type;
}Formal_P;

/* Nodo de la lista de parametros formales */
typedef struct Node_P_List{
    Formal_P p;
    struct Node_P_List* next;
}Node_P_List;

/* Lista de parametros formales */
typedef struct Formal_P_List{
    Node_P_List* head;
    int size;
}Formal_P_List;

/* Nodo de la lista de parametros reales */
typedef struct Node_C_List {
    node* p;
    struct Node_C_List* next;
}Node_C_List;

/* Lista de parametros reales */
typedef struct Current_P_List{
    Node_C_List* head;
    int size;
}Current_P_List;

/* -- Funciones para manipular una la lista de parametros formales -- */
Formal_P new_arg(char* name, VarType type, int value);
void insert_f_param(Formal_P_List** f_params, Formal_P a);

/* -- Funcion para manipular la lista de parametros actuales -- */
void insert_c_param(Current_P_List** c_params, node* expr);
char* list_to_string(Formal_P_List* f_params);

void print_c_params(Current_P_List* c_params, FILE* parser_out);

// Funcion para imprimir una expresion
void print_expr(node* expr, FILE* parser_out);

#endif