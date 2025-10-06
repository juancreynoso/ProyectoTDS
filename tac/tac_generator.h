// tac_generator.h
#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include <stdio.h>
#include "../common.h"
#include "../ast/ast.h"

// Tipos de instrucciones
typedef enum{
    PRG,
    END_PRG,
    FC,
    RET,
    ASSIGN,
    PLUS,
    SUB,
    MULT,
    DIV,
    REST,
    MINUS,
    GT,
    LT,
    EQUALS,
    AND,
    OR,
    NOT
}instruction_type;

// Tipos de operandos
typedef enum{
    OPE_DECL_METH,
    OPE_TEMP,
    OPE_VAR,
    OPE_NUM,
    OPE_BOOL
    // Llamadas a metodos ??
}op_class;

typedef struct operand{
    op_class class;
    char* name;
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

char* new_temp();

void tac_code(node* root, FILE* tac_out);
operand gen_tac_code(node* root, instruction_list **list);

void traverse_ast_for_tac(node* root, instruction_list **list); 

char* instruction_representation(instruction i);
char* operand_to_str(operand op);

instruction_list* init_instruction_list();
void insert_instruction(instruction_list** list, instruction i);
void save_instruction_list(instruction_list* list, FILE* tac_out);

char* op_to_tr(instruction_type type);
instruction_type op_name_to_inst_type(OpType type);



#endif