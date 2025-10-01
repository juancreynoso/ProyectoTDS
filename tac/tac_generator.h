// tac_generator.h
#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include "../common.h"
#include "../ast/ast.h"

// Tipos de instrucciones
typedef enum{
    FC,
    FFC,
    SAVE,
    ADD,
}instruction_type;

// Estructura para cada instruccion
typedef struct instruction{
    instruction_type type;
    union type *op1;             
    union type *op2;
    union type *result; 
}instruction;

// Lista de instrucciones
typedef struct instruction_list{
    instruction i;
    struct instruction_list* next;
}instruction_list;

#endif