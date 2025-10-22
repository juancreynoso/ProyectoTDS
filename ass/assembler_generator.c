#include <stdio.h>
#include <string.h>
#include "assembler_generator.h"


/**
 @param list
 @param ass_out
 */
void ass_gen(instruction_list* list, FILE* ass_out) {
    instruction_node* cursor = list->head;

    // Buffer para cada seccion
    char data_segment[8192] = {0};
    char text_segment[8192] = {0};

    // Punteros para escribir sobre cada seccion
    char* data_ptr = data_segment;
    char* text_ptr = text_segment;

    while(cursor){
        instruction_to_assembler(cursor->i, &data_ptr, &text_ptr);
        cursor = cursor->next;
    }

    printf(" Cantidad de instrucciones: %d \n", list->size);
    
    // concatenacion en el archivo ass_out
    fprintf(ass_out, ".data\n%s\n.text\n%s", data_segment, text_segment);
}

char* instruction_to_assembler(instruction i, char** data_ptr, char** text_ptr){
    switch(i.type){
        case FUNC:
            if (strcmp("main", i.op1.name) == 0) {
                *text_ptr += sprintf(*text_ptr, ".globl main\n");
                *text_ptr += sprintf(*text_ptr, "%s: \n", i.op1.name);
            } else {
                *text_ptr += sprintf(*text_ptr, "%s: \n", i.op1.name);
            }
            int frame_size = i.op1.info->METH_DECL.frame_size;
            *text_ptr += sprintf(*text_ptr, "    enter  $%d, $0\n", frame_size);
            break;
        case FFUNC:
            *text_ptr += sprintf(*text_ptr, "    leave\n"); 
            *text_ptr+= sprintf(*text_ptr, "    ret\n");
            break;
         case ASSIGN:
            if (i.op1.info->ID.is_glbl == 1) {
                if (i.op1.class == OPE_VAR_USE) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %s(%%rip)\n",  i.op2.info->INT.value, i.op1.info->ID.name); 
                } else {
                    *data_ptr += sprintf(*data_ptr, "    %s: .quad %d\n",  i.op1.info->ID.name, i.op2.info->INT.value); 
                }
            }else {
                if (i.op2.class == OPE_NUM) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op2.info->INT.value);
                    *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);  
                } else if (i.op2.class == OPE_BOOL) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op2.info->BOOL.value);
                    *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);                  
                } else if (i.op2.class == OPE_TEMP){
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
                    *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset);  
                } else if (i.op2.class == OPE_VAR_USE) {
                    if (i.op2.info->ID.is_glbl == 1) {
                        *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op2.info->ID.name); 
                    } else {
                        *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset); 
                    }                
                    *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n",  i.op1.info->ID.offset); 
                }     
            }   
            break;
         case PLUS:
             if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    add $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    add %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    add %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    add %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }
            *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case SUB:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    sub $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    sub %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    sub %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    sub %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case MULT:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    imul $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    imul %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    imul %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    imul %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case DIV:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%rax\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    xor %%rdx, %%rdx\n");

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    idiv %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%rax, %d(%%rbp)\n", i.result.info->OP.offset);
            break;   
        case REST:

            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rbp), %%rax\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    xor %%rdx, %%rdx\n");

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    idiv %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%rdx, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
//         case MINUS:
//             // if (i.op1.class == OPE_NUM ) {
//             //     fprintf(ass_out, "    mov $%d, %%r10\n", i.op1.info->INT.value);
//             //     fprintf(ass_out, "    neg %%r10\n");
//             // }  else if (i.op1.class == OPE_TEMP){
//             //     fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
//             //     fprintf(ass_out, "    neg %%r10\n");
//             // } else if (i.op1.class == OPE_VAR_USE) {
//             //     fprintf(ass_out, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
//             //     fprintf(ass_out, "    neg %%r10\n");
//             // }

//             // fprintf(ass_out, "    movq %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
        
//             // break;
        case GT:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    cmp %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }                    
            }

            *text_ptr += sprintf(*text_ptr, "    mov $1, %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    mov $0, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    cmovg %%r10, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);
            
            break;
        case LT:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    cmp %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    mov $1, %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    mov $0, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    cmovl %%r10, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case EQUALS:
            if (i.op1.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_NUM ) {
                *text_ptr += sprintf(*text_ptr, "    cmp $%d, %%r10\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    cmp %s(%%rip), %%r10\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    cmp %d(%%rbp), %%r10\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    mov $1, %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    mov $0, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    cmove %%r10, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);

            break;

        case NOT:
            if (i.op1.class == OPE_BOOL ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }  

            *text_ptr += sprintf(*text_ptr, "    test %%r10, %%r10\n");
            *text_ptr += sprintf(*text_ptr, "    setz %%r10b\n");
            *text_ptr += sprintf(*text_ptr, "    movzb %%r10b, %%r10\n");
            
            *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n", i.result.info->OP.offset);
            break;

        case AND:
            if (i.op1.class == OPE_BOOL ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_BOOL ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r11\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r11\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r11\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r11\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    and %%r10, %%r11\n");
            *text_ptr += sprintf(*text_ptr,"    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);

            break;
        case OR:
            if (i.op1.class == OPE_BOOL ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
            }  else if (i.op1.class == OPE_TEMP){
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                }
            }

            if ( i.op2.class == OPE_BOOL ) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r11\n", i.op2.info->INT.value);
            } else if ( i.op2.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r11\n", i.op2.info->OP.offset);
            } else if (i.op2.class == OPE_VAR_USE) {
                if (i.op2.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r11\n", i.op2.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r11\n", i.op2.info->ID.offset);
                }
            }

            *text_ptr += sprintf(*text_ptr, "    or %%r10, %%r11\n");
            *text_ptr += sprintf(*text_ptr, "    mov %%r11, %d(%%rbp)\n", i.result.info->OP.offset);
            break;
        case LOAD: {
            const char* regs[] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
            int index = i.op2.info->INT.value;   // índice del parámetro

            if (index < 6) {
                if (i.op1.class == OPE_NUM) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %s\n", i.op1.info->INT.value, regs[index]);
                } else if (i.op1.class == OPE_VAR_USE) {
                    if (i.op1.info->ID.is_glbl == 1) {
                        *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %s\n", i.op1.info->ID.name, regs[index]);
                    } else {
                        *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %s\n", i.op1.info->ID.offset, regs[index]);
                    }
                } else if (i.op1.class == OPE_TEMP) {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %s\n", i.op1.info->OP.offset, regs[index]);
                } else if (i.op1.class == OPE_BOOL) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %s\n", i.op1.info->BOOL.value, regs[index]);
                }
            } else {
                if (i.op1.class == OPE_NUM) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->INT.value);
                } else if (i.op1.class == OPE_VAR_USE) {
                    if (i.op1.info->ID.is_glbl == 1) {
                        *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%r10\n", i.op1.info->ID.name);
                    } else {
                        *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->ID.offset);
                    }
                } else if (i.op1.class == OPE_TEMP) {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", i.op1.info->OP.offset);
                } else if (i.op1.class == OPE_BOOL) {
                    *text_ptr += sprintf(*text_ptr, "    mov $%d, %%r10\n", i.op1.info->BOOL.value);
                }
                *text_ptr += sprintf(*text_ptr, "    push %%r10\n");
            }
            break;
        }
        case PARAM: {
            const char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            static int index = 0;
            
            if (index < 6) {
                if (i.op1.class == OPE_PARAM) { // guardar parametro
                    *text_ptr += sprintf(*text_ptr, "    mov %s, %d(%%rbp)\n", regs[index], i.op1.info->ID.offset);
                }
            } else {
                int param_stack_offset = 16 + 8 * (index - 6);
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%r10\n", param_stack_offset);
                *text_ptr += sprintf(*text_ptr, "    mov %%r10, %d(%%rbp)\n", i.op1.info->ID.offset);
            }
            index++;
            break;
        }
        case RET: {
            int offset;
            if (i.op1.class == OPE_VAR_USE){
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%rax\n", i.op1.info->ID.name);
                    break;
                } else {
                    offset = i.op1.info->ID.offset;
                }
            } else if (i.op1.class == OPE_TEMP) {
                offset = i.op1.info->OP.offset;
            } else if (i.op1.class == OPE_NUM){
               offset = i.op1.info->INT.value; 
               *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", offset);
               break;
            } else {
               offset = i.op1.info->BOOL.value; 
               *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", offset);
               break;                
            }
            *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", offset);

            break; 
        }   
        case IF_COND:
            if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%rax\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
                }
            } else if (i.op1.class == OPE_TEMP){
               *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            }
            *text_ptr += sprintf(*text_ptr, "    cmp $1, %%rax\n");
            *text_ptr += sprintf(*text_ptr, "    je .%s\n", i.op2.name);

            break;
        case IF_FALSE_GOTO: {
            if (i.op1.class == OPE_NUM) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", i.op1.info->INT.value);
            } else if (i.op1.class == OPE_VAR_USE) {
                if (i.op1.info->ID.is_glbl == 1) {
                    *text_ptr += sprintf(*text_ptr, "    mov %s(%%rip), %%rax\n", i.op1.info->ID.name);
                } else {
                    *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->ID.offset);
                }
            } else if (i.op1.class == OPE_TEMP) {
                *text_ptr += sprintf(*text_ptr, "    mov %d(%%rbp), %%rax\n", i.op1.info->OP.offset);
            } else if (i.op1.class == OPE_BOOL) {
                *text_ptr += sprintf(*text_ptr, "    mov $%d, %%rax\n", i.op1.info->BOOL.value);
            }
            
            *text_ptr += sprintf(*text_ptr, "    cmp $0, %%rax\n");
            *text_ptr += sprintf(*text_ptr, "    je .%s\n", i.op2.name);
            break;
        }  
        case LABEL:
            *text_ptr += sprintf(*text_ptr, ".%s:\n", i.op1.name);
            break;
        case GOTO:
            *text_ptr += sprintf(*text_ptr, "    jmp .%s\n", i.op1.name);
            break;
        case CALL:
            *text_ptr += sprintf(*text_ptr, "    call %s\n", i.op1.name);
            break;         
        default:
            break;
    }
}
