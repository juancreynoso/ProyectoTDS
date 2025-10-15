#include <stdio.h>
#include "set_offsets.h"

static int var_offset = 0;
static int param_offset = 8;

void reset_offsets(){
    var_offset = 0;
    param_offset = 8;    
}

int new_var_offset() {
    var_offset -= 8;
    return var_offset;
}

int new_param_offset() {
    param_offset += 8;
    return param_offset;
}

void set_offsets(node* root) {
    if (root == NULL) {
        return;
    }

    switch(root->type) {
        case NODE_DECL:
            root->info->ID.offset = new_var_offset(); 
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
        default:
            printf("voy por default \n");
            set_offsets(root->left);
            set_offsets(root->right);
            break;
    }
}