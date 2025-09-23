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

/**
 * Construye una cadena que representa la lista de parametros
 */ 
char* list_to_string(Formal_P_List* f_params) {
    char *result = malloc(1024);
    result[0] = '\0';

    if (f_params == NULL) {
        strcat(result, "()");
        return result;
    }

    Node_P_List* cursor = f_params->head;
    
    strcat(result, "(");
    while (cursor != NULL) {
        char buffer[128];

        sprintf(buffer, "%s %s", var_type_to_string(cursor->p.type), cursor->p.name);
        strcat(result, buffer);
        if (cursor->next != NULL) {
            strcat(result, ", ");
        }
        cursor = cursor->next;
    }
    strcat(result, ")");
    return result;
}

/**
 * Imprime un arbol que representa una expresion
 */
void expr_to_str(node* root){
    if (root == NULL) {
        return;
    }
    switch(root->type){
        case NODE_NUM:
            printf("%d", root->info->INT.value);
            break;
        case NODE_BOOL:
            printf("%s", root->info->BOOL.value ? "true" : "false");
            break;
        case NODE_ID_USE:
            printf("%s", root->info->ID.name);
            break;
        case NODE_CALL_METH:
            printf("%s", root->info->METH_CALL.name);
            print_c_params(root->info->METH_CALL.c_params);
            break;
        case NODE_OP:
            switch (root->info->OP.name) {
                case OP_GT:
                    expr_to_str(root->left);
                    printf(" > ");
                    expr_to_str(root->right);
                    break;
                case OP_LT:
                    expr_to_str(root->left);
                    printf(" < "); 
                    expr_to_str(root->right); 
                    break;                               
                case OP_PLUS: 
                    expr_to_str(root->left);
                    printf(" + "); 
                    expr_to_str(root->right);
                    break;
                case OP_SUB: 
                    expr_to_str(root->left);
                    printf(" - "); 
                    expr_to_str(root->right);
                    break;
                case OP_MULT: 
                    expr_to_str(root->left);
                    printf(" * "); 
                    expr_to_str(root->right);
                    break;
                case OP_DIV: 
                    expr_to_str(root->left);
                    printf(" / "); 
                    expr_to_str(root->right);
                    break;
                case OP_REST: 
                    expr_to_str(root->left);
                    printf("%%\n"); 
                    expr_to_str(root->right);
                    break;
                case OP_MINUS: 
                    printf(" - "); 
                    expr_to_str(root->left);
                    break;                 
                case OP_EQUALS: 
                    expr_to_str(root->left);
                    printf(" == ");
                    expr_to_str(root->right);  
                    break;                           
                case OP_NOT: 
                    printf("!"); 
                    expr_to_str(root->left); 
                    break;                
                case OP_OR:
                    expr_to_str(root->left); 
                    printf(" || "); 
                    expr_to_str(root->right); 
                    break;
                case OP_AND: 
                    expr_to_str(root->left);
                    printf(" && "); 
                    expr_to_str(root->right);
                    break;
                default: 
                    printf(" OP? "); 
                    break;
            }
        default: printf("NO es una expresion"); break;
    }

}

/**
 * Imprime los argumentos que se pasan en la llamada de un metodo
 */
void print_c_params(Current_P_List* c_params){
    if (c_params == NULL) {
        printf("()");
        return;
    }
    Node_C_List* cursor = c_params->head;

    printf("(");
    while (cursor != NULL) {
        expr_to_str(cursor->p);
        if (cursor->next != NULL) {
            printf(", ");
        }
        cursor = cursor->next;
    }

    printf(")");
}

/**
 *  Crear un parametro formal
 */
Formal_P new_arg(char* name, VarType type, int value){
    Formal_P a;
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

/**
 * Insertar un parametro en la lista de parametros formales
 */
void insert_f_param(Formal_P_List** f_params, Formal_P a){
        Formal_P_List* new = malloc(sizeof(Formal_P_List));
        new->head = malloc(sizeof(Node_P_List));

        new->head->p.name = a.name;
        new->head->p.type = a.type;
        new->head->next = NULL;

    if (*f_params == NULL) {
        *f_params = new;
        (*f_params)->size = 1; 
    } else {
        Node_P_List* temp = (*f_params)->head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new->head;
        (*f_params)->size++; 
    }
}

/**
 * Insertar un parametro en la lista de parametros reales
 */ 
void insert_c_param(Current_P_List** c_params, node* expr){
    Current_P_List* new = malloc(sizeof(Current_P_List));
    new->head = malloc(sizeof(Node_P_List));
    new->head->p = expr;
    new->head->next = NULL;

    if (*c_params ==  NULL) {
        *c_params = new;
        (*c_params)->size = 1; 
    } else {
        Node_C_List* temp = (*c_params)->head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new->head;
        (*c_params)->size++; 
    }
}

/* -----  Constructores de nodos ----- */

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

/**
 * Crea un nodo correspondiente a una operacion
 */
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
/**
 * Crea un nodo correspondiente a una sentencia if then else
 */
node* create_if_else_node(node* expr, node* if_block, node* else_block){
    node* root = new_node(NODE_IF_ELSE);
    root->info->IF_ELSE.expr = expr;
    root->info->IF_ELSE.if_block = if_block;
    root->info->IF_ELSE.else_block = else_block;

    return root;
}

/**
 * Crea un nodo correspondiente a una sentencia while
 */
node* create_while_node(node* expr, node* block){
    node* root = new_node(NODE_WHILE);
    root->info->WHILE.expr = expr;
    root->info->WHILE.block = block;
    
    return root;
}

/**
 * Crea un nodo correspondiente a la declaracion de un metodo
 */
node* create_meth_decl_node(char* name, Formal_P_List* f_params, VarType returnType, int is_extern){
    node* root = new_node(NODE_DECL_METH);
    root->info->METH_DECL.name = name;
    root->info->METH_DECL.f_params = f_params;
    root->info->METH_DECL.returnType = returnType;
    root->info->METH_DECL.is_extern = is_extern;
    
    return root;
}

/**
 * Crea un nodo correspondiente a la llamada de un metodo
 */
node* create_meth_call_node(char*name, Current_P_List* c_params){
    node* root = new_node(NODE_CALL_METH);
    root->info->METH_CALL.name = name;
    root->info->METH_CALL.c_params = c_params;
    return root;
}

node* create_block_node(char* name) {
    node* root = new_node(NODE_BLOCK);
    root->info->BLOCK_INFO.name = name;
    
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
 * Crea un nodo que se utiliza para llevar informacion sobre el ast
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
            case OP_GT: printf(">\n"); break;
            case OP_LT: printf("<\n"); break;                                
            case OP_PLUS: printf("+\n"); break;
            case OP_SUB: printf("-\n"); break;
            case OP_MULT: printf("*\n"); break;
            case OP_DIV: printf("/\n"); break;
            case OP_REST: printf("%%\n"); break;
            case OP_MINUS: printf("-\n"); break;                
            case OP_ASSIGN: printf("=\n"); break; 
            case OP_EQUALS: printf("==\n"); break;                           
            case OP_NOT: printf("!\n"); break;                
            case OP_OR: printf("||\n"); break;
            case OP_AND: printf("&&\n"); break;
            default: printf("OP?\n"); break;
            }
            break;
        case NODE_RET: printf("ret \n"); break;
        case NODE_DECL:
            switch(root->info->ID.type){
                case TYPE_INT: printf("int "); break;
                case TYPE_BOOL: printf("bool "); break;
                case NONE: printf("none "); break;
            }
            printf("%s\n", root->info->ID.name ? root->info->ID.name : "NULL");
            break;
        case NODE_ID_USE:
            printf("%s\n", root->info->ID.name ? root->info->ID.name : "NULL");
            break;
        case NODE_DECL_METH:
            if (root->info->METH_DECL.is_extern  == 1) {
                printf("extern ");
            }
            switch(root->info->METH_DECL.returnType) {
                case TYPE_INT:
                    printf("int ");
                    printf("%s", root->info->METH_DECL.name);
                    break;
                case TYPE_BOOL:
                    printf("bool ");
                    printf("%s", root->info->METH_DECL.name);
                    break; 
                case NONE:
                    printf("void ");
                    printf("%s", root->info->METH_DECL.name);
                    break;
            }
            printf("%s size: %d \n ", list_to_string(root->info->METH_DECL.f_params), root->info->METH_DECL.f_params ? root->info->METH_DECL.f_params->size : 0 );      
            break;
        case NODE_CALL_METH:
            printf(" %s",root->info->METH_CALL.name);
            print_c_params(root->info->METH_CALL.c_params);
            printf(" size: %d", root->info->METH_CALL.c_params ? root->info->METH_CALL.c_params->size : 0 );
            printf("\n");
            break;
        case NODE_IF_ELSE:
            printf("if \n");
            print_tree(root->info->IF_ELSE.expr, level + 1);
            print_tree(root->info->IF_ELSE.if_block, level+1);
            print_tree(root->info->IF_ELSE.else_block, level+1);
            break;
        case NODE_WHILE:
            printf("while \n");
            print_tree(root->info->WHILE.expr, level+1);
            print_tree(root->info->WHILE.block, level+1);
            break;
        case NODE_PYC: printf("PYC"); break;
        case NODE_INFO:
             printf("%s\n", root->info->NODE_INFO.info ? root->info->NODE_INFO.info : "NULL");
            break;
        case NODE_BLOCK: 
            printf("block \n");
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


