#include "constants_propagation.h"
#include <stdlib.h>

node* optimize_constants(node* root) {
    if (root == NULL) {
        return NULL;
    }

    root->left = optimize_constants(root->left);
    root->right = optimize_constants(root->right);

    if (root->type == NODE_OP && root->left && root->right)  {
        if (root->left->type == NODE_NUM && root->right->type == NODE_NUM) {
            int a = root->left->info->INT.value;
            int b = root->right->info->INT.value;
            int result;

            switch (root->info->OP.name) {
                case OP_PLUS:  
                    result = a + b;
                    break;
                case OP_SUB:   
                    result = a - b;
                    break;
                case OP_MULT:  
                    result = a * b;
                    break;
                case OP_DIV:
                    result = a / b;
                    break;
                case OP_REST:  
                    result = a % b;
                    break;
                default: 
                    return root;
            }

            node* new_node = create_node(NULL, TYPE_INT, root->line, root->column);
            new_node->type = NODE_NUM;
            new_node->info->INT.value = result;
            free(root->left);
            free(root->right);

            return new_node;

        } else if (root->left->type == NODE_BOOL && root->right->type == NODE_BOOL) {
            int a = root->left->info->INT.value;
            int b = root->right->info->INT.value;
            int result;

            switch (root->info->OP.name) {
                case OP_AND:  
                    result = a && b;
                    break;
                case OP_OR:   
                    result = a || b;
                    break;
                case OP_EQUALS:
                    result = a == b;
                    break;
                default: 
                    return root;
            }

            node* new_node = create_node(NULL, TYPE_BOOL, root->line, root->column);
            new_node->type = NODE_BOOL;
            new_node->info->BOOL.value = result;
            free(root->left);
            free(root->right);

            return new_node;
        }
    }
    return root;
}
