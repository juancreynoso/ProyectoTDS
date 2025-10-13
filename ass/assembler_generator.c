#include <stdio.h>
#include <string.h>
#include "assembler_generator.h"


/**
 @param list
 @param ass_out
 */
void ass_gen(instruction_list* list, FILE* ass_out) {
    instruction_node* cursor = list->head;


    while(cursor){
        instruction_to_assembler(cursor->i, ass_out);
        cursor = cursor->next;
    }

    printf(" Cantidad de instrucciones: %d \n", list->size);
    
}


char* instruction_to_assembler(instruction i, FILE* ass_out){
    switch(i.type){
        case FUNC:
            if (strcmp("main", i.op1.name) == 0) {
                fprintf(ass_out, "    .globl main\n");
                fprintf(ass_out, "%s: \n", i.op1.name);
            } else {
                fprintf(ass_out, "%s: \n", i.op1.name);
            }
            fprintf(ass_out, "    push %%rbp\n");
            fprintf(ass_out, "    mov %%rsp %%rbp\n");
            break;
        case FFUNC:
            fprintf(ass_out, "    leave\n");
            fprintf(ass_out, "    ret\n");
            break;
        case PLUS:
        case SUB:
        case MULT:
        case DIV:
        case REST:
        case MINUS:
        case GT:
        case LT:
        case EQUALS:
        case AND:
        case OR:
            fprintf(ass_out, "    %s offset %d \n", i.result.name,i.result.offset);
            break;
        case LOAD:
            fprintf(ass_out, "    %s offset %d \n", i.op1.name, i.op1.offset);
            break;
        // case:
        //     break;                    
        default:
            break;
    }
}