#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"
#include "semantic_analyzer.h"

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
typedef struct tables_stack {
    node_s* top;
} tables_stack;

void insert_symbol(symbol_table **table, symbol s, NodeType nodeType);
union type* search_symbol(tables_stack *stack, char* name, NodeType type);
union type* search_in_table(symbol_table* table, char* name);
void print_symbol_table(symbol_table *table, FILE* semantic_out);
tables_stack* create_stack();
void push(tables_stack* stack, symbol_table* table);
symbol_table* pop(tables_stack* stack);

#endif