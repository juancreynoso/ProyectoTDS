#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "ast.h"

/*
    Estructura de los simbolos que se guardan en la tabla
*/
typedef struct symbol {
    union type *info;
} symbol;

/*
    Lista enlazada que implementa los ambitos del programa
*/
typedef struct symbol_table {
    NodeType nodeType;
    symbol s;
    struct symbol_table *next;
} symbol_table;

/*
    Nodos del stack de tablas
*/
typedef struct node_s {
    symbol_table* data;
    struct node_s* next;
} node_s;

/*
    Stack que implementa la tabla de simbolos
*/
typedef struct tables_stack {   // Stack
    node_s* top;             // apunta al tope de la pila (cabeza de la lista)
} tables_stack;

void semantic_analysis_recursive(node* root, tables_stack* stack, symbol_table* table, node* parent);
tables_stack* analyze_semantics(node* root);
void insert_symbol(symbol_table **table, symbol s, NodeType nodeType);
union type* search_symbol(tables_stack *stack, char* name);
union type* search_in_table(symbol_table* table, char* name);
void print_symbol_table(symbol_table *table);
void add_formal_params_to_scope(tables_stack* stack, Formal_P_List* f_params);
void check_current_params(Current_P_List* c_params, tables_stack* stack, symbol_table* table);

tables_stack* create_stack();
void push(tables_stack* stack, symbol_table* table);
symbol_table* pop(tables_stack* stack);

#endif