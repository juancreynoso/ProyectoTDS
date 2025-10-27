#include <stdio.h>
#include "set_offsets.h"

static int var_offset = 0;
static int param_offset = 8;
static int max_offset = 0;

/**
 * Resetea los offsets
 */
void reset_offsets(){
    var_offset = 0;
    param_offset = 8;    
}

/**
 * Genera un nuevo offset para una variable o temporal
 */
int new_var_offset() {
    var_offset -= 8;
    if (var_offset < max_offset) {
        max_offset = var_offset; 
    }
    return var_offset;
}

/**
 * Genera un nuevo offset para un parametro
 */
int new_param_offset() {
    param_offset += 8;
    return param_offset;
}

int get_frame_size() {
    return -max_offset;
}

/**
 * Se encarga de asignar offsets tanto a variables, parametros y temporales, para la generacion de assembler.
 * @param root Nodo raiz del AST.
 */
void set_offsets(node* root) {
    if (root == NULL) {
        return;
    }

    switch(root->type) {
        case NODE_DECL:
            root->info->ID.offset = new_var_offset(); 
            break;
        case NODE_CALL_METH:
            root->info->METH_CALL.offset = new_var_offset(); 
            if (root->info->METH_CALL.c_params != NULL) {
                Node_C_List* cursor = root->info->METH_CALL.c_params->head;
                while (cursor != NULL) {
                    set_offsets_actual_params(cursor->p);
                    cursor = cursor->next;
                }
            }
            break;
        case NODE_OP:
            switch(root->info->OP.name) {
                case OP_PLUS:
                case OP_SUB:
                case OP_MULT:
                case OP_DIV:
                case OP_REST:
                case OP_MINUS:
                case OP_GT:
                case OP_LT:
                case OP_EQUALS:
                case OP_AND:
                case OP_OR:
                case OP_NOT:
                    root->info->OP.offset = new_var_offset();
                    set_offsets(root->right);
                    set_offsets(root->left);
                    break;
                case OP_ASSIGN:
                    set_offsets(root->right);
                    set_offsets(root->left);
                    break;
            }
            break;
        case NODE_IF_ELSE:
            set_offsets(root->info->IF_ELSE.expr);
            set_offsets(root->info->IF_ELSE.if_block);
            set_offsets(root->info->IF_ELSE.else_block);
            break;
        case NODE_WHILE:
            set_offsets(root->info->WHILE.block);
            set_offsets(root->info->WHILE.expr);
            break;
        case NODE_DECL_METH:
            reset_offsets();
            int count = 0;
            if (root->info->METH_DECL.f_params == NULL) {
                set_offsets(root->left);
                root->info->METH_DECL.frame_size = get_frame_size();
                break;
            }

            Node_P_List* cursor =  root->info->METH_DECL.f_params->head;
            
            while (cursor != NULL) {
                if (count < 6) {
                    cursor->p.offset = new_var_offset();
                    count++;
                } else {
                    cursor->p.offset = new_param_offset();
                }
                cursor = cursor->next;
            }

            set_offsets(root->left);


            root->info->METH_DECL.frame_size = get_frame_size();
            break;
        default:
            set_offsets(root->left);
            set_offsets(root->right);
            break;
    }
}

/**
 * Se encarga de asignar offsets a los sub arboles que representan parametros actuales.
 * @param root Nodo raiz del subarbol que representa una expresion.
 */
void set_offsets_actual_params(node* root) {
    if (root == NULL) {
        return ;
    }    

    switch(root->type){
        case NODE_OP:
            root->info->OP.offset = new_var_offset();
            break;
        case NODE_CALL_METH:
            set_offsets(root);
            break;
        default:
            set_offsets_actual_params(root->left);
            set_offsets_actual_params(root->right);
            break;
    }
}