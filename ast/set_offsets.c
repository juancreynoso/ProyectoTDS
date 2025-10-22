#include <stdio.h>
#include "set_offsets.h"

static int var_offset = 0;
static int param_offset = 8;
static int max_offset = 0;

void reset_offsets(){
    var_offset = 0;
    param_offset = 8;    
}

int new_var_offset() {
    var_offset -= 8;
    if (var_offset < max_offset) {
        max_offset = var_offset; 
    }
    return var_offset;
}

int new_param_offset() {
    param_offset += 8;
    return param_offset;
}

int get_frame_size() {
    return -max_offset;
}

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
                    printf("Gen este offset %d\n", root->info->OP.offset);
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
                printf("Tamaño maximo de frame del metodo %s: %d\n",root->info->METH_DECL.name, root->info->METH_DECL.frame_size);
                break;
            }

            Node_P_List* cursor =  root->info->METH_DECL.f_params->head;
            
            while (cursor != NULL) {
                if (count < 6) {
                    cursor->p.offset = new_var_offset();
                    printf("Offset para parámetro %s: %d\n", cursor->p.name, cursor->p.offset);
                    count++;
                } else {
                    cursor->p.offset = new_param_offset();
                    printf("Offset para parámetro %s: %d\n", cursor->p.name, cursor->p.offset);
                }
                cursor = cursor->next;
            }

            set_offsets(root->left);


            root->info->METH_DECL.frame_size = get_frame_size();
            printf("Tamaño maximo de frame del metodo %s: %d\n",root->info->METH_DECL.name, root->info->METH_DECL.frame_size);
            break;
        default:
            set_offsets(root->left);
            set_offsets(root->right);
            break;
    }
}