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
                fprintf(ass_out, "\n    .globl main\n");
                fprintf(ass_out, "%s: \n", i.op1.name);
            } else {
                fprintf(ass_out, "%s: \n", i.op1.name);
            }
            int frame_size = i.op1.info->METH_DECL.frame_size;
            fprintf(ass_out, "    enter  $%d, $0\n", frame_size);
            break;
        case FFUNC:
            fprintf(ass_out, "    leave\n"); 
            fprintf(ass_out, "    ret\n");
            break;
        case ASSIGN:
            if (i.op2.class == OPE_NUM) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op2.info->INT.value);
                fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);  
            } else if (i.op2.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
                fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);  
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);                  
            }        
            break;
        case PLUS:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    %s mov %d(%%rbp), %%r10\n", i.op1.info->ID.name, i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    add $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    add %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    add %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case SUB:
        case MULT:
        // case DIV:
        // case REST:
        // case MINUS:
        // case GT:
        // case LT:
        // case EQUALS:
        // case AND:
        // case OR:
            fprintf(ass_out, "    temp offset %d \n",i.op1.info->OP.offset);
            break;
//        case LOAD:
            //fprintf(ass_out, "    %s offset %d \n", i.op1.name, i.op1.offset);
            break;
         case RET:{
            int offset;
            if (i.op1.class == OPE_VAR){
                offset = i.op1.info->ID.offset;
            } else {
                offset = i.op1.info->OP.offset;
            }
            fprintf(ass_out, "    mov %d(%%rbp), %%rax,\n", offset);

            break; 
         }         
        case CALL:
            fprintf(ass_out, "    CALL %s\n", i.op1.name);
            break;         
        default:
            break;
    }
}
