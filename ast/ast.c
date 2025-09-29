#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/**
 * Funcion que construye una cadena que representa la lista de parametros
 * @param f_params Lista de parametros formales a convertir
 * @return Cadena que representa la lista de parametros
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

        sprintf(buffer, "%s %s", type_to_string(cursor->p.type), cursor->p.name);
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
 * Funcion que imprime un arbol que representa una expresion
 * @param root Nodo raiz del sub arbol a imprimir 
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
            break;
        default: printf("NO es una expresion"); break;
    }

}

/**
 * Funcion que imprime los argumentos que se pasan en la llamada de un metodo
 * @param c_params
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
 * Funcion que crea un parametro formal.
 * @param name Lista de parametros formales.
 * @param type Tipo de dato asociado al parametro.
 * @param value Valor asociado al parametro.
 * @return Nuevo parametro.
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
 * Funcion que inserta un parametro en la lista de parametros formales.
 * @param f_params Lista de parametros formales.
 * @param a Parametro a insertar.
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
 * Funcion que inserta un parametro en la lista de parametros reales.
 * @param c_params Lista de parametros reales.
 * @param expr Parametro a insertar.
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
 * Funcion que crea un nodo correspondiente a una constante entera.
 * @param value Valor de la constante entera.
 * @return Nuevo nodo creado.
 */
node* create_int_node(int value){
    node* root = new_node(NODE_NUM);
    root->info->INT.type = TYPE_INT;
    root->info->INT.value = value;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a un valor booleano.
 * @param value Valor entero que representa el valor booleano.
 * @return Nuevo nodo creado.
 */
node* create_bool_node(int value){
    node* root = new_node(NODE_BOOL);
    root->info->INT.type = TYPE_BOOL;
    root->info->INT.value = value;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a una operacion.
 * @param name Tipo de operacion.
 * @param type Tipo de dato asociado a esa operacion.
 * @return Nuevo nodo creado
 */
node* create_op_node(OpType name, VarType type){
    node* root = new_node(NODE_OP);
    root->info->OP.name = name;
    root->info->ID.type = type;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a la ocurrencia de un ID (declaracion o expresion).
 * @param name Nombre del ID.
 * @param typeVar Tipo de dato asociado al ID.
 * @param type Tipo de nodo (ID_USE, DECL).
 * @return Nuevo nodo creado.
 */
node* create_id_node(char* name, VarType typeVar, NodeType type){
    node* root = new_node(type);
    root->info->ID.name = name;
    root->info->ID.type = typeVar;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a una sentencia IF THEN / IF THEN ELSE.
 * @param expr Sub arbol que representa la expresion de condicion del IF.
 * @param if_block Sub arbol que representa el bloque de sentencias contenidas en IF THEN
 * @param else_block Sub arbol que representa el bloque de sentencias contenidas en el bloque ELSE
 * @return Nuevo nodo creado.
 */
node* create_if_else_node(node* expr, node* if_block, node* else_block){
    node* root = new_node(NODE_IF_ELSE);
    root->info->IF_ELSE.expr = expr;
    root->info->IF_ELSE.if_block = if_block;
    root->info->IF_ELSE.else_block = else_block;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a una sentencia WHILE.
 * @param expr Sub arbol que representa la expresion de condicion del ciclo.
 * @param block Sub arbol que representan el conjunto de sentencias contenidas en el bloque del ciclo.
 * @return Nuevo nodo creado.
 */
node* create_while_node(node* expr, node* block){
    node* root = new_node(NODE_WHILE);
    root->info->WHILE.expr = expr;
    root->info->WHILE.block = block;
    
    return root;
}

/**
 * Funcion que crea un nodo correspondiente a la declaracion de un metodo.
 * @param name Nombre del metodo.
 * @param f_params Lista de parametros formales.
 * @param returnType Tipo de retorno del metodo.
 * @param is_extern Valor que indica si el metodo es externo (1) o no (0)
 * @return Nuevo nodo creado.
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
 * Funcion que crea un nodo correspondiente a la llamada de un metodo.
 * @param name Nombre del metodo.
 * @param c_params Lista de parametros reales.
 * @return Nuevo nodo creado.
 */
node* create_meth_call_node(char*name, Current_P_List* c_params){
    node* root = new_node(NODE_CALL_METH);
    root->info->METH_CALL.name = name;
    root->info->METH_CALL.c_params = c_params;
    return root;
}

/**
 * Funcion que crea un nodo correspondiente un bloque de sentencias.
 * @return Nuevo nodo creado.
 */
node* create_block_node() {
    node* root = new_node(NODE_BLOCK);
    
    return root;
}

/**
 * Funcion que crea un nodo correspondiente una sentencia return.
 * @param type Tipo de retorno.
 * @return Nuevo nodo creado.
 */
node* create_return_node(VarType type){
    node* root = new_node(NODE_RET);
    root->info->RETURN.type = type;

    return root;
}

/**
 * Funcion que crea un nodo que se utiliza para llevar informacion sobre el AST.
 * @param info cadena que lleva informacion del nodo.
 * @param type tipo de dato que llevara el nodo.
 * @return Nuevo nodo creado.
 */
node* create_node(char* info, VarType type){
    node* root = new_node(NODE_INFO);
    root->info->NODE_INFO.type = type;
    root->info->NODE_INFO.info = info;

    return root;
}

/**
 * Funcion que crea un nodo de un tipo en particular.
 * @param type Tipo de nodo.
 * @return Nuevo nodo creado.
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
 * Funcion que crea un arbol a partir de un nodo raiz y dos sub arboles.
 * @param root Nodo raiz.
 * @param left Nodo izquierdo.
 * @param right Nodo derecho.
 * @return Nuevo nodo creado.
 */
node* create_tree(node* root, node* left, node* right){
    root->left = left;
    root->right = right;
    return root;
}

/**
 * Funcion que imprime los distintos nodos del arbol.
 * @param root Nodo raiz.
 * @param level Indica el indice del espaciado para imprimir correctamente.
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
 * Funcion de entrada que se encarga de imprimir el arbol.
 * @param root Tipo de nodo.
 * @param level Indica el indice del espaciado para imprimir correctamente.
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


