#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

/**
 * Constructores de nodos
 */

/**
 * Crea un nodo correspondiente a una constante entera
 */
node* create_int_node(int value){
    node* root = new_node(NODE_NUM);
    root->info->INT.type = TYPE_INT
    root->info->INT.value = value;

    return root;
}

/**
 * Crea un nodo correspondiente a valor booleano
 */
node* create_bool_node(int value){
    node* root = new_node(NODE_BOOL);
    root->info->INT.type = TYPE_BOOL
    root->info->INT.value = value;

    return root;
}

node* create_id_node(char* name, VarType typeVar, NodeType type){
    node* root = new_node(type);
    root->info->ID.name = name;
    root->info->ID.type = typeVar;

    return root;
}

/**
 * Crea un nodo correspondiente a una sentencia return
 */
node* create_return_node(VarType type){
    node* root = new_node(NODE_RET);
    root->info->RETURN.type = type;

    return root;
}

node* new_node(NodeType type){
    node* root = malloc(sizeof(node));
    root->info = malloc(sizeof(union type));
    root->type = type;
    root->left = NULL;
    root->right = NULL;
    return root;
}

/**
 * Crea un arbol a partir de un nodo raiz y dos sub arboles
 */ 

node* create_tree(node* root, node* left, node* right){
    root.left = left;
    root.right = right;
    return root;
}

/**
 * Imprime los distintos nodo del arbols
 */

void print_node(node *root) {
    if (!root)
        return;
    switch (root->type) {
        case NODE_NUM:
            printf("%d\n", root->info->NUM.value);
            break;
        case NODE_DECL:
            switch(root->info->ID.type){
                case TYPE_INT:
                    printf("int ");
                    break;
                case TYPE_BOOL:
                    printf("bool ");
                    break;
                case NONE:
                    printf("none ");
                    break;
            }
            printf("%s\n", root->info->ID.name ? root->info->ID.name : "NULL");
            break;
        case NODE_ID_USE:
            printf("%s\n", root->info->ID.name ? root->info->ID.name : "NULL");
            break;
        case NODE_BOOL:
            printf("%s\n", root->info->BOOL.value ? "true" : "false");
            break;
        case NODE_FUNC:
            switch(root->info->FUNC.returnType) {
                case TYPE_INT:
                    printf("int ");
                    printf("%s \n", root->info->FUNC.name);
                    break;
                case TYPE_BOOL:
                    printf("bool ");
                    printf("%s \n", root->info->FUNC.name);
                    break; 
                case NONE:
                    printf("void ");
                    printf("%s \n", root->info->FUNC.name);
                    break;
            }
            break;
        case NODE_OP:
            switch (root->info->OP.name) {
            case OP_ADD:
                printf("+\n");
                break;
            case OP_SUB:
                printf("-\n");
                break;
            case OP_MULT:
                printf("*\n");
                break;
            case OP_DIV:
                printf("/\n");
                break;
            case OP_ASSIGN:
                printf("=\n");
                break;
            case OP_OR:
                printf("||\n");
                break;
            case OP_AND:
                printf("&&\n");
                break;
            default:
                printf("OP?\n");
            }
            break;
        case NODE:
             printf("%s\n", root->info->NODE.info ? root->info->NODE.info : "NULL");
            break;
        case NODE_RET:
            printf("ret \n");
            break;
        default:
            printf("UNKNOWN NODE\n");
            break;
        }
}

/**
 * Funcion de entrada que se encarga de imprimir el arbol
 */

void print_tree(node *root, int level) {
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++){
        printf("   ");
    }

    print_node(root);
    print_tree(root->left, level + 1);
    print_tree(root->right, level + 1);
}
