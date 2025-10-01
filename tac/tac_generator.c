#include <stdio.h>
#include <stdlib.h>
#include "tac_generator.h"


void tac_code(node* root, FILE* tac_out){
    instruction_list* list = NULL;

    gen_tac_code(root, list, tac_out);
    save_instruction_list(list, tac_out);
}

union type* gen_tac_code(node* root, instruction_list *list){
    if (root == NULL) {
        return;
    }

    switch(root->type){
        case NODE_NUM:
        case NODE_BOOL:
        case NODE_ID_USE:
            return root->info;
            break;    
        case NODE_RET:
            union type* t1  = gen_tac_code(root->left, list);
            instruction i;
            i.type = RET;
            i.op1 = t1;
            insert_instruction(&list, i);
            break;
        case NODE_OP:
            switch(root->info->OP.name) {
                case OP_ASSIGN:
                    union type* right = gen_tac_code(root->right, list);
                    union type* left = gen_tac_code(root->left, list);
                    union type* t1;

                    instruction i;
                    i.type = SAVE;
                    i.op1 = left; 
                    i.op2 = right;
                    i.result = t1;
                    insert_instruction(&list, i);

                    return NULL;
                    break;
                case OP_PLUS:
                    union type* left = gen_tac_code(root->left, list);
                    union type* right = gen_tac_code(root->right, list);
                    union type* t1;

                    instruction i;
                    i.type = PLUS;
                    i.op1 = left; 
                    i.op2 = right;
                    i.result = t1;
                    insert_instruction(&list, i);

                    return NULL;
                    break;
                default:
                    break;
            }
            break;
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
    instruction_list* new = malloc(sizeof(instruction_list));
    new->head = malloc(sizeof(instruction_node));
    new->head->i = i;
    new->head->next = NULL;

    if (*list == NULL) {
        *list = new;
        (*list)->size = 1;
    } else {
        instruction_node* temp = (*list)->head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new->head;
        (*list)->size++;
    }
}


void save_instruction_list(instruction_list* list,  FILE* tac_out) {
    if (list == NULL) {
        return;
    }

    instruction_node* cursor = list->head;

    while(cursor != NULL){
        instruction i = cursor->i;
        char* i_str = instruction_representation(i);
        fprinf(tac_out, i);
        cursor = cursor->next;
    }

}

char* instruction_representation(instruction i){
    switch(i.type){
        case SAVE:
            break;
        case PLUS:
            break;
        default:
            break;
    }
}
