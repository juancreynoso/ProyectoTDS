#include <stdio.h>
#include <stdlib.h>
#include "tac_generator.h"


static int temp_count = 0;

char* new_temp() {
    char* name = malloc(10);
    sprintf(name, "t%d", temp_count++);
    return name;
}

void tac_code(node* root, FILE* tac_out){
    instruction_list* list = NULL;

    traverse_ast_for_tac(root, &list);
    save_instruction_list(list, tac_out);
}

// Recorre solamente 
void traverse_ast_for_tac(node* root, instruction_list **list) {
    if (root == NULL) {
        return;
    }

    switch(root->type){
        case NODE_OP:
            switch(root->info->OP.name){
                case OP_ASSIGN: {
                    operand t1 = gen_tac_code(root->right, list); 
                    operand id = gen_tac_code(root->left, list);

                    instruction i;
                    i.type = ASSIGN;
                    i.op1 = id; 
                    i.op2 = t1;

                    insert_instruction(list, i);

                    break;                    
                }
                default:
                    printf("Entre por otra operacion \n");
                    break;
            }
            break;
        case NODE_RET: {
            operand t1 = gen_tac_code(root->left, list);
            instruction i;
            i.type = RET;
            i.op1 = t1;
            insert_instruction(list, i);

            break;
        }

        default:
            traverse_ast_for_tac(root->left, list);
            traverse_ast_for_tac(root->right, list);
            break;
    }

}

operand gen_tac_code(node* root, instruction_list **list){

    switch(root->type){
        
        case NODE_NUM: {
            operand op;
            op.class = OP_NUM;
            op.info = root->info;
            return op;

            break;
        }

        case NODE_BOOL: {
            operand op;            
            op.class = OP_BOOL;
            op.info = root->info;
            return op;

            break;
        } 

        case NODE_DECL:
        case NODE_ID_USE: {
            operand op;
            printf(" Este es el nombre: %s \n", root->info->ID.name);
            op.class = OP_VAR;
            op.info = root->info;
            return op;
            break;    
        }
            
        case NODE_OP: {
            switch(root->info->OP.name) {
            case OP_PLUS : {
                operand left = gen_tac_code(root->left, list);
                operand right = gen_tac_code(root->right, list);
                operand t1;
                t1.class = OP_TEMP;
                t1.info = malloc(sizeof(union type));
                t1.info->ID.name = new_temp();
                t1.info->ID.type = root->info->OP.type;

                instruction i;
                i.type = PLUS;
                i.op1 = left; 
                i.op2 = right;
                i.result = t1;
                insert_instruction(list, i);

                return t1;
                break;
            }

            case OP_SUB : {
                operand left = gen_tac_code(root->left, list);
                operand right = gen_tac_code(root->right, list);
                operand t1;
                t1.class = OP_TEMP;
                t1.info = malloc(sizeof(union type));
                t1.info->ID.name = new_temp();
                t1.info->ID.type = root->info->OP.type;

                instruction i;
                i.type = SUB;
                i.op1 = left; 
                i.op2 = right;
                i.result = t1;
                insert_instruction(list, i);

                return t1;
                break;
            }
                
            case OP_MULT : {
                operand left = gen_tac_code(root->left, list);
                operand right = gen_tac_code(root->right, list);
                operand t1;
                t1.class = OP_TEMP;
                t1.info = malloc(sizeof(union type));
                t1.info->ID.name = new_temp();
                t1.info->ID.type = root->info->OP.type;

                instruction i;
                i.type = MULT;
                i.op1 = left; 
                i.op2 = right;
                i.result = t1;
                insert_instruction(list, i);

                return t1;
                break;
            }      

            case OP_DIV : {
                operand left = gen_tac_code(root->left, list);
                operand right = gen_tac_code(root->right, list);
                operand t1;
                t1.class = OP_TEMP;
                t1.info = malloc(sizeof(union type));
                t1.info->ID.name = new_temp();
                t1.info->ID.type = root->info->OP.type;

                instruction i;
                i.type = DIV;
                i.op1 = left; 
                i.op2 = right;
                i.result = t1;
                insert_instruction(list, i);

                return t1;
                break;
            }                  

            default:
                break;
            }
            break;
        }
        default:
            gen_tac_code(root->left, list);
            gen_tac_code(root->right, list);
            break;
    }

 }

/**
 * Funcion que inserta una instruccion en una lista de instrucciones
 * @param list Lista de instrucciones.
 * @param i Nueva instruccion.
 */
void insert_instruction(instruction_list** list, instruction i){
    
    instruction_node* new_it = malloc(sizeof(instruction_node));
    new_it->i = i;
    new_it->next = NULL;

    if (*list == NULL) {
        *list = malloc(sizeof(instruction_list));
        (*list)->head = new_it;
        (*list)->size = 1;
    } else {
        instruction_node* temp = (*list)->head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_it;
        (*list)->size++;
    }
}


void save_instruction_list(instruction_list* list,  FILE* tac_out) {
    if (list == NULL) {
        return;
    }

    instruction_node* cursor = list->head;

    printf("Llego acaaa\n");
    while(cursor != NULL){
        instruction i = cursor->i;
        char* i_str = instruction_representation(i);
        fprintf(tac_out, "%s", i_str);
        free(i_str);
        cursor = cursor->next;
    }

}

char* operand_to_str(operand op) {

    char* buffer = malloc(64);
    switch(op.class) {
        case OP_NUM:
            snprintf(buffer, 64, "%d", op.info->INT.value);
            break;
        case OP_BOOL:
            snprintf(buffer, 64, "%s", op.info->BOOL.value ? "true" : "false");
            break;
        case OP_VAR:
        case OP_TEMP:
            snprintf(buffer, 64, "%s", op.info->ID.name);
            break;
        default:
            snprintf(buffer, 64, "???");
            break;
    }
    return buffer;
}

char* instruction_representation(instruction i) {
    char* buffer = malloc(128);
    
    char* op1_str;
    char* op2_str;
    char* result_str;

    printf("Representacion \n");

    switch(i.type) {
        case RET: {
            printf("RET\n");
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "RET %s\n", op1_str);
            free(op1_str);
            break;
        }
        case PLUS:
        case SUB:
        case MULT:
        case DIV:
        case REST:
        case GT:
        case LT:
        case EQUALS:
        case AND:
        case OR:
            op1_str = operand_to_str(i.op1);
            op2_str = operand_to_str(i.op2);
            result_str = operand_to_str(i.result);
            snprintf(buffer, 128, "%s := %s %s %s\n",
                    result_str,
                    op1_str,
                    op_to_tr(i.type),
                    op2_str);
            free(op1_str);
            free(op2_str);
            free(result_str);
            break;
        // case MINUS:
        //     op1_str = operand_to_str(i.op1);
        //     result_str = operand_to_str(i.result);
        //     snprintf(buffer, 128, "%s := %s %s\n",
        //             result_str,
        //             op1_str,
        //             op_name(i.type);
        //             op2_str);
        //     free(op1_str);
        //     free(op2_str);
        //     free(result_str);
        // case NOT:
        case ASSIGN:
            printf("SAVE\n");
            op1_str = operand_to_str(i.op1);
            op2_str = operand_to_str(i.op2);
             snprintf(buffer, 128, "%s := %s\n",
                     op1_str,
                     op2_str);
            free(op1_str);
            free(op2_str);
            break;
        default:
            snprintf(buffer, 128, "UNKNOWN\n");
    }

    return buffer;
}

 char* op_to_tr(instruction_type type) {
    switch(type){
        case PLUS:
            return "PLUS";
        case SUB:
            return "SUB";
        case MULT:
            return "MULT";
        case DIV:
            return "DIV";
        case REST:
            return "REST";
        case GT:
            return "GT";
        case LT:
            return "LT";
        case EQUALS:
            return "EQUALS";
        case AND:
            return "AND";
        case OR:
            return "OR";
        case MINUS:
            return "MINUS";
        case NOT:
            return "NOT";
        default:
            return "?";
            break;
    }
 }