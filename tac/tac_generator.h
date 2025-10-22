// tac_generator.h
#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include <stdio.h>
#include "../common.h"
#include "../ast/ast.h"

// Tipos de instrucciones
typedef enum {
    PRG,
    END_PRG,
    FUNC,
    FFUNC,
    IF_COND,
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
    LABEL,
    GOTO,
    IF_FALSE_GOTO,
    CALL,
    PARAM,
    LOAD,
    NOT
} instruction_type;

// Tipos de operandos
typedef enum {
    OPE_DECL_METH,
    OPE_CALL_METH,
    OPE_TEMP,
    OPE_VAR_DECL,
    OPE_VAR_USE,
    OPE_NUM,
    OPE_BOOL,
    OPE_LABEL,
    OPE_PARAM
} op_class;

/* Estructura de un operando */
typedef struct operand {
    op_class class;
    char* name;
    union type *info;
} operand;

/* Estructura de una instruccion */
typedef struct instruction {
    instruction_type type;
    operand op1;             
    operand op2;
    operand result; 
} instruction;

/* Nodo de la lista de instrucciones */
typedef struct instruction_node {
    instruction i;
    struct instruction_node* next;
} instruction_node;

/* Lista de instrucciones */
typedef struct instruction_list {
    instruction_node* head;
    int size;
} instruction_list;

/* Funciones necesarias para crear nuevos temporales y labels */
char* new_temp();
char* new_label();

/* Funciones principales para generar el codigo tres direcciones*/
instruction_list* tac_code(node* root, FILE* tac_out);
void generate_tac_from_ast(node* root, instruction_list **list); 
operand generate_tac_from_expression(node* root, instruction_list **list);

/* Funciones para manipular la lista de instrucciones */
instruction_list* init_instruction_list();
void insert_instruction(instruction_list** list, instruction i);

/* Funciones para mostrar y guardar las instrucciones en un formato mas legible */
void save_instruction_list(instruction_list* list, FILE* tac_out);
char* instruction_representation(instruction i);
char* operand_to_str(operand op);
char* op_to_tr(instruction_type type);
instruction_type op_name_to_inst_type(OpType type);

#endif