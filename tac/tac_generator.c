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
    instruction_list* list = init_instruction_list();

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
                    i.type = op_name_to_inst_type(root->info->OP.name);
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
            op.class = OPE_NUM;
            op.info = root->info;
            return op;

            break;
        }

        case NODE_BOOL: {
            operand op;            
            op.class = OPE_BOOL;
            op.info = root->info;
            return op;

            break;
        } 

        case NODE_DECL:
        case NODE_ID_USE: {
            operand op;
            printf(" Este es el nombre: %s \n", root->info->ID.name);
            op.class = OPE_VAR;
            op.info = root->info;
            return op;
            break;    
        }
            
        case NODE_OP: {
            switch(root->info->OP.name) {
                case OP_PLUS: 
                case OP_SUB:
                case OP_MULT:
                case OP_DIV: 
                case OP_REST:
                case OP_GT:
                case OP_LT:
                case OP_EQUALS:
                case OP_AND:
                case OP_OR:{
                    operand left = gen_tac_code(root->left, list);
                    operand right = gen_tac_code(root->right, list);
                    operand t1;
                    t1.class = OPE_TEMP;
                    t1.info = malloc(sizeof(union type));
                    t1.info->ID.name = new_temp();
                    t1.info->ID.type = root->info->OP.type;

                    instruction i;
                    i.type = op_name_to_inst_type(root->info->OP.name);
                    i.op1 = left; 
                    i.op2 = right;
                    i.result = t1;
                    insert_instruction(list, i);

                    return t1;
                    break;
                }
                case OP_NOT:
                case OP_MINUS: {
                    operand left = gen_tac_code(root->left, list);
                    operand t1;
                    t1.class = OPE_TEMP;
                    t1.info = malloc(sizeof(union type));
                    t1.info->ID.name = new_temp();
                    t1.info->ID.type = root->info->OP.type;

                    instruction i;
                    i.type = op_name_to_inst_type(root->info->OP.name);
                    i.op1 = left; 
                    i.result = t1;
                    insert_instruction(list, i);

                    return t1;
                    break;
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
void insert_instruction(instruction_list** list, instruction i) {

    instruction_node* new_it = malloc(sizeof(instruction_node));
    new_it->i = i;
    new_it->next = NULL;

    instruction_node* temp = (*list)->head;

    while (temp->next->i.type != END_PRG) {
        temp = temp->next;
    }

    new_it->next = temp->next;
    temp->next = new_it;

    (*list)->size++;
}


instruction_list* init_instruction_list() {
    instruction_list* list = malloc(sizeof(instruction_list));
    list->head = NULL;
    list->size = 0;

    // Crear las instrucciones PROGRAM y END
    instruction i_begin;
    instruction i_end;
    i_begin.type = PRG;
    i_end.type = END_PRG;

    // Crear nodos
    instruction_node* n_begin = malloc(sizeof(instruction_node));
    instruction_node* n_end = malloc(sizeof(instruction_node));

    n_begin->i = i_begin;
    n_end->i = i_end;
    n_begin->next = n_end;
    n_end->next = NULL;

    list->head = n_begin;
    list->size = 2;

    return list;
}



void save_instruction_list(instruction_list* list,  FILE* tac_out) {
    if (list == NULL) {
        return;
    }

    instruction_node* cursor = list->head;

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
        case OPE_NUM:
            snprintf(buffer, 64, "%d", op.info->INT.value);
            break;
        case OPE_BOOL:
            snprintf(buffer, 64, "%s", op.info->BOOL.value ? "true" : "false");
            break;
        case OPE_VAR:
        case OPE_TEMP:
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
        case PRG:
            printf("INIT\n");
            snprintf(buffer, 128, "PROGRAM\n");
            break;
        case END_PRG:
            printf("END\n");
            snprintf(buffer, 128, "END\n");
            break;     
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
        case MINUS:
        case NOT:
            op1_str = operand_to_str(i.op1);
            result_str = operand_to_str(i.result);
            snprintf(buffer, 128, "%s := %s %s\n",
                    result_str,
                    op_to_tr(i.type),
                    op1_str);
            free(op1_str);
            free(result_str);
            break;
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
            break;
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

 instruction_type op_name_to_inst_type(OpType type) {
    switch(type){
        case OP_ASSIGN:
            return ASSIGN;
            break;
        case OP_PLUS:
            return PLUS;
            break;
        case OP_SUB:
            return SUB;
            break;
        case OP_MULT:
            return MULT;
            break;
        case OP_DIV:
            return DIV;
            break;
        case OP_REST:
            return REST;
            break;
        case OP_MINUS:
            return MINUS;
            break;
        case OP_GT:
            return GT;
            break;
        case OP_LT:
            return LT;
            break;
        case OP_EQUALS:
            return EQUALS;
            break;
        case OP_AND:
            return AND;
            break;
        case OP_OR:
            return OR;
            break;
        case OP_NOT:
            return NOT;
            break;
    }
 }
