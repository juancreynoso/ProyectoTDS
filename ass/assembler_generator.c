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
            } else if (i.op2.class == OPE_BOOL) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op2.info->BOOL.value);
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
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
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
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    sub $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    sub %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    sub %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case MULT:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    imul $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    imul %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    imul %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case DIV:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
            }

            fprintf(ass_out, "    xor %%rdx, %%rdx\n");

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    idiv %%r10\n");
            fprintf(ass_out, "    mov %%rax, %d(%%rbp)\n", i.result.info->OP.offset);
            break;   
        case REST:

            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
            }

            fprintf(ass_out, "    xor %%rdx, %%rdx\n");

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    idiv %%r10\n");
            fprintf(ass_out, "    mov %%rdx, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case MINUS:
            // if (i.op1.class == OPE_NUM ) {
            //     fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            //     fprintf(ass_out, "    neg %%r10\n");
            // }  else if (i.op1.class == OPE_TEMP){
            //     fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            //     fprintf(ass_out, "    neg %%r10\n");
            // } else if (i.op1.class == OPE_VAR) {
            //     fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            //     fprintf(ass_out, "    neg %%r10\n");
            // }

            // fprintf(ass_out, "    movq %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
        
            // break;
        case GT:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov $1, %%r10\n");
            fprintf(ass_out, "    mov $0, %%r11\n");
            fprintf(ass_out, "    cmovg %%r10, %%r11\n");
            fprintf(ass_out, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case LT:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov $1, %%r10\n");
            fprintf(ass_out, "    mov $0, %%r11\n");
            fprintf(ass_out, "    cmovl %%r10, %%r11\n");
            fprintf(ass_out, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case EQUALS:
            if (i.op1.class == OPE_NUM ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_NUM ) {
                fprintf(ass_out, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    mov $1, %%r10\n");
            fprintf(ass_out, "    mov $0, %%r11\n");
            fprintf(ass_out, "    cmove %%r10, %%r11\n");
            fprintf(ass_out, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
//      case NOT:
        case AND:
            if (i.op1.class == OPE_BOOL ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_BOOL ) {
                fprintf(ass_out, "    mov $%d, %%r11\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r11\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r11\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    and %%r10, %%r11\n");
            fprintf(ass_out, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case OR:
            if (i.op1.class == OPE_BOOL ) {
                fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR) {
                printf("Aca %d\n", i.op1.info->ID.offset);
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
            }

            if ( i.op2.class == OPE_BOOL ) {
                fprintf(ass_out, "    mov $%d, %%r11\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r11\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%r11\n", i.op2.info->ID.offset);
            }

            fprintf(ass_out, "    or %%r10, %%r11\n");
            fprintf(ass_out, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case LOAD: {
            const char* regs[] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
            int index = i.op2.info->INT.value;   // índice del parámetro

            if (index < 6) {
                if (i.op1.class == OPE_NUM) {
                    fprintf(ass_out, "    mov $%d, %s\n", i.op1.info->INT.value, regs[index]);
                } else if (i.op1.class == OPE_VAR) {
                    fprintf(ass_out, "    mov %d(%%rbp), %s\n", i.op1.info->ID.offset, regs[index]);
                } else if (i.op1.class == OPE_TEMP) {
                    fprintf(ass_out, "    mov %d(%%rbp), %s\n", i.op1.info->OP.offset, regs[index]);
                } else if (i.op1.class == OPE_BOOL) {
                    fprintf(ass_out, "    mov $%d, %s\n", i.op1.info->BOOL.value, regs[index]);
                }
            } else {
                if (i.op1.class == OPE_NUM) {
                    fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
                } else if (i.op1.class == OPE_VAR) {
                    fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                } else if (i.op1.class == OPE_TEMP) {
                    fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
                } else if (i.op1.class == OPE_BOOL) {
                    fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->BOOL.value);
                }
                fprintf(ass_out, "    push %%r10\n");
            }
            break;
        }
        case PARAM: {
            const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            static int index = 0;
            
            if (index < 6) {
                if (i.op1.class == OPE_VAR) { // guradar parametro
                    fprintf(ass_out, "    mov %s, %d(%%rbp)\n", regs[index], i.op1.info->ID.offset);
                }
            } else {
                int param_stack_offset = 16 + 8 * (index - 6);
                fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", param_stack_offset);
                fprintf(ass_out, "    mov %%r10, %d(%%rbp)\n", i.op1.info->ID.offset);
            }
            
            break;
        }
        case RET: {
            int offset;
            if (i.op1.class == OPE_VAR){
                offset = i.op1.info->ID.offset;
            } else if (i.op1.class == OPE_TEMP) {
                offset = i.op1.info->OP.offset;
            } else if (i.op1.class == OPE_NUM){
               offset = i.op1.info->INT.value; 
               fprintf(ass_out, "    mov $%d, %%rax\n", offset);
               break;
            } else {
               offset = i.op1.info->BOOL.value; 
               fprintf(ass_out, "    mov $%d, %%rax\n", offset);
               break;                
            }
            fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", offset);

            break; 
        }   
        case IF_COND:
            if (i.op1.class == OPE_VAR) {
                fprintf(ass_out,    "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
            } else if (i.op1.class == OPE_TEMP){
               fprintf(ass_out,    "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            }
            fprintf(ass_out,    "    cmp $1, %%rax\n");
            fprintf(ass_out, "    je .%s\n", i.op2.name);

            break;
        case IF_FALSE_GOTO: {
            if (i.op1.class == OPE_NUM) {
                fprintf(ass_out, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            } else if (i.op1.class == OPE_VAR) {
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
            } else if (i.op1.class == OPE_TEMP) {
                fprintf(ass_out, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_BOOL) {
                fprintf(ass_out, "    mov $%d, %%rax\n", i.op1.info->BOOL.value);
            }
            
            fprintf(ass_out, "    cmp $0, %%rax\n");
            fprintf(ass_out, "    je .%s\n", i.op2.name);
            break;
        }  
        case LABEL:
            fprintf(ass_out, ".%s:\n", i.op1.name);
            break;
        case GOTO:
            fprintf(ass_out, "    jmp .%s\n", i.op1.name);
            break;
        case CALL:
            fprintf(ass_out, "    call %s\n", i.op1.name);
            break;         
        default:
            break;
    }
}
