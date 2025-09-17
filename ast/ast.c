#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/** 
 * Devuelve un string por cada valor del enumerado VariableType 
 * Utilizado para mostrar resultados mas claros 
 */
char* var_type_to_string(VarType type){
    switch(type){
        case TYPE_INT:
            return "int";
            break;
        case TYPE_BOOL:
            return "bool";
            break; 
        case NONE:
            return "void";
            break;
    }
}

char* list_to_string(Args_List* args) {
    char *result = malloc(1024);
    result[0] = '\0';

    Args_List* cursor = args;
    
    strcat(result, "[ ");
    while (cursor != NULL) {
        char buffer[128];

        sprintf(buffer, "%s %s", var_type_to_string(cursor->p.type), cursor->p.name);
        strcat(result, buffer);
        if (cursor->next != NULL) strcat(result, ", "); // separador
        cursor = cursor->next;
    }
    strcat(result, " ]");
    return result;
}

/* Constructores de nodos */
/**
 * Crea un nodo correspondiente a una constante entera
 */
node* create_int_node(int value){
    node* root = new_node(NODE_NUM);
    root->info->INT.type = TYPE_INT;
    root->info->INT.value = value;

    return root;
}

/**
 * Crea un nodo correspondiente a valor booleano
 */
node* create_bool_node(int value){
    node* root = new_node(NODE_BOOL);
    root->info->INT.type = TYPE_BOOL;
    root->info->INT.value = value;

    return root;
}

node* create_op_node(OpType name, VarType type){
    node* root = new_node(NODE_OP);
    root->info->OP.name = name;
    root->info->ID.type = type;

    return root;
}

/**
 * Crea un nodo correspondiente a la ocurrencia de un ID (declaracion o expresion)
 */
node* create_id_node(char* name, VarType typeVar, NodeType type){
    node* root = new_node(type);
    root->info->ID.name = name;
    root->info->ID.type = typeVar;

    return root;
}

node* create_if_else_node(node* expr, node* if_block, node* else_block){
    node* root = new_node(NODE_IF_ELSE);
    root->info->IF_ELSE.expr = expr;
    root->info->IF_ELSE.if_block = if_block;
    root->info->IF_ELSE.else_block = else_block;

    return root;
}

node* create_while_node(node* expr, node* block){
    node* root = new_node(NODE_WHILE);
    root->info->WHILE.expr = expr;
    root->info->WHILE.block = block;
    
    return root;
}

/**
 * Crea un nodo correspondiente a un metodo
 */
node* create_meth_node(char* name, Args_List* arguments, VarType returnType, NodeType type, int is_extern){
    node* root = new_node(type);
    root->info->METH.name = name;
    root->info->METH.arguments = arguments;
    root->info->METH.returnType = returnType;
    root->info->METH.is_extern = is_extern;
    
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

/**
 * Crea un nodo que se utiliza para llevar info sobre el ast
 */
node* create_node(char* info, VarType type){
    node* root = new_node(NODE_INFO);
    root->info->NODE_INFO.type = type;
    root->info->NODE_INFO.info = info;

    return root;
}

/**
 * Recibe un tipo de nodo y lo crea
 * Esta funcion se utiliza para crear nodos de un tipo en especifico
 */
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
    root->left = left;
    root->right = right;
    return root;
}

Arg new_arg(char* name, VarType type, int value){
    Arg a;
    a.name = name;
    a.type = type;

    switch(type){
        case TYPE_INT:
            break;
        case TYPE_BOOL:
            break;
        default:
            break;
    }
    return a;
}

void insert_arg(Args_List** list, Arg a){
        Args_List* new = malloc(sizeof(Args_List));
        new->p.name = a.name;
        new->p.type = a.type;
        new->next = NULL;
    if (*list == NULL) {
        *list = new;
    } else {
        Args_List* temp = *list;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
    }
}
   
/**
 * Imprime los distintos nodo del arbols
 */
void print_node(node *root, int level) {
    if (!root)
        return;
    switch (root->type) {
        case NODE_NUM:
            printf("%d\n", root->info->INT.value);
            break;
        case NODE_BOOL:
            printf("%s\n", root->info->BOOL.value ? "true" : "false");
            break;
        case NODE_OP:
            switch (root->info->OP.name) {
            case OP_GT:
                printf(">\n");
                break;
            case OP_LT:
                printf("<\n");
                break;                                
            case OP_PLUS:
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
            case OP_MINUS:
                printf("-\n");
                break;                
            case OP_ASSIGN:
                printf("=\n");
                break; 
            case OP_EQUALS:
                printf("==\n");
                break;                           
            case OP_NOT:
                printf("!\n");
                break;                
            case OP_OR:
                printf("||\n");
                break;
            case OP_AND:
                printf("&&\n");
                break;
            default:
                printf("OP?\n");
                break;
            }
            break;
        case NODE_RET:
            printf("ret \n");
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
        case NODE_METH:
            switch(root->info->METH.returnType) {
                case TYPE_INT:
                    printf("int ");
                    printf("%s ", root->info->METH.name);
                    break;
                case TYPE_BOOL:
                    printf("bool ");
                    printf("%s ", root->info->METH.name);
                    break; 
                case NONE:
                    printf("void ");
                    printf("%s ", root->info->METH.name);
                    break;
            }
            if (root->info->METH.arguments != NULL) {
                printf("%s \n", list_to_string(root->info->METH.arguments));
            } else {
                printf("\n");
            }      
            break;
        case NODE_CALL_METH:
            printf("call %s",root->info->METH.name);
            break;
        case NODE_IF_ELSE:
            printf("if \n");
            print_tree(root->info->IF_ELSE.expr, level + 1);
            print_tree(root->info->IF_ELSE.if_block, level+1);
            print_tree(root->info->IF_ELSE.else_block, level+1);
            break;
        case NODE_WHILE:
            printf("while \n");
            print_node(root->info->WHILE.expr, level+1);
            print_node(root->info->WHILE.block, level+1);
            break;
        case NODE_PYC:
            printf("PYC");
            break;
        case NODE_INFO:
             printf("%s\n", root->info->NODE_INFO.info ? root->info->NODE_INFO.info : "NULL");
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

    print_node(root, level);
    print_tree(root->left, level + 1);
    print_tree(root->right, level + 1);
}


