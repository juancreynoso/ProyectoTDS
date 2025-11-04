#include "optimizations.h"
#include <stdlib.h>

/**
 * Recorre el AST y simplifica expresiones compuestas por constantes por el valor que denotan.
 * @param root nodo raiz del AST.
 */
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

/**
 * Recorre el AST y elimina los subarboles que representan codigo siguiente a un return en el mismo bloque.
 * @param root nodo raiz del AST.
 */
void remove_dead_code(node* root) {
    if (root == NULL) {
        return;
    }

    if (root->type == NODE_INFO && root->left->type == NODE_RET) {
        delete_subtree(root->right);
        root->right = NULL;
    } 

    switch(root->type) {
        case NODE_IF_ELSE:
            remove_dead_code(root->info->IF_ELSE.if_block);
            remove_dead_code(root->info->IF_ELSE.else_block);
            break;
        case NODE_WHILE:
            remove_dead_code(root->info->WHILE.block);
            break;
        default:
            remove_dead_code(root->left);
            remove_dead_code(root->right);
            break;
    }
}

/**
 * Funcion principal que se encarga de ejecutar las optimizaciones disponibles sobre el AST
 * @param root Nodo raiz del AST.
 */
void run_optimization(node* root) {
    optimize_constants(root);
    remove_dead_code(root);
    return;
}

/**
 * Se encarga de eliminar un sub arbol del AST.
 * @param root Nodo raiz del sub arbol que se liberará.
 */
void delete_subtree(node* root) {
    if (root == NULL) {
        return;
    }

    if (root->info) {
        free(root->info);
    }

    delete_subtree(root->left);
    delete_subtree(root->right);
    free(root);
}
