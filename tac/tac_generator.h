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
    PLUS,
}instruction_type;

typedef enum{
    OP_TEMP,
    OP_VAR_USE,
    OP_VAR_DECL,
    OP_NUM,
    OP_BOOL
}op_class;

typedef struct operand{
    op_class class;
    union type *info;
}operand;

// Estructura para cada instruccion
typedef struct instruction{
    instruction_type type;
    operand op1;             
    operand op2;
    operand result; 
}instruction;

// Nodo de la lista de instrucciones
typedef struct instruction_node{
    instruction i;
    struct instruction_node* next;
}instruction_node;

typedef struct instruction_list{
    instruction_node* head;
    int size;
}instruction_list;

void tac_code(node* root, FILE* tac_out);
operand gen_tac_code(node* root, instruction_list *list);

char* instruction_representation(instruction i);

void insert_instruction(instruction_list** list, instruction i);
void save_instruction_list(instruction_list* list, FILE* tac_out);

#endif