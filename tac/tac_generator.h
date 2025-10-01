// tac_generator.h
#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include <stdio.h>
#include "../common.h"
#include "../ast/ast.h"

// Tipos de instrucciones
typedef enum{
    FC,
    FFC,
    SAVE,
    RET,
}instruction_type;

// Estructura para cada instruccion
typedef struct instruction{
    instruction_type type;
    union type *op1;             
    union type *op2;
    union type *result; 
}instruction;

// Nodo de la lista de instrucciones
typedef struct instruction_node{
    instruction i;
    struct instruction_list* next;
}instruction_node;

typedef struct instruction_list{
    instruction_node* head;
    int size;
}instruction_list;

void tac_code(node* root, FILE* tac_out);
void gen_tac_code(node* root, instruction_list *list, FILE* tac_out);

void insert_instruction(instruction_list** list, instruction i);
void save_instruction_list(instruction_list* list, FILE* tac_out);

#endif