#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* -----  Constructores de nodos ----- */

/**
 * Funcion que crea un nodo correspondiente a una constante entera.
 * @param value Valor de la constante entera.
 * @return Nuevo nodo creado.
 */
node* create_int_node(int value, int line, int column){
    node* root = new_node(NODE_NUM, line, column);
    root->info->INT.type = TYPE_INT;
    root->info->INT.value = value;

    return root;
}

/**
 * Funcion que crea un nodo correspondiente a un valor booleano.
 * @param value Valor entero que representa el valor booleano.
 * @return Nuevo nodo creado.
 */
node* create_bool_node(int value, int line, int column){
    node* root = new_node(NODE_BOOL, line, column);
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
node* create_op_node(OpType name, VarType type, int line, int column){
    node* root = new_node(NODE_OP, line, column);
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
node* create_id_node(char* name, VarType typeVar, NodeType type, int line, int column){
    node* root = new_node(type, line, column);
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
node* create_if_else_node(node* expr, node* if_block, node* else_block, int line, int column){
    node* root = new_node(NODE_IF_ELSE, line, column);
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
node* create_while_node(node* expr, node* block, int line, int column){
    node* root = new_node(NODE_WHILE, line, column);
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
node* create_meth_decl_node(char* name, Formal_P_List* f_params, VarType returnType, int is_extern, int line, int column){
    node* root = new_node(NODE_DECL_METH, line, column);
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
node* create_meth_call_node(char*name, Current_P_List* c_params, int line, int column){
    node* root = new_node(NODE_CALL_METH, line, column);
    root->info->METH_CALL.name = name;
    root->info->METH_CALL.c_params = c_params;
    return root;
}

/**
 * Funcion que crea un nodo correspondiente un bloque de sentencias.
 * @return Nuevo nodo creado.
 */
node* create_block_node(char* name, int line, int column) {
    node* root = new_node(NODE_BLOCK, line, column);
    root->info->BLOCK_INFO.name = name;
    return root;
}

/**
 * Funcion que crea un nodo correspondiente una sentencia return.
 * @param type Tipo de retorno.
 * @return Nuevo nodo creado.
 */
node* create_return_node(VarType type, int line, int column){
    node* root = new_node(NODE_RET, line, column);
    root->info->RETURN.type = type;

    return root;
}

/**
 * Funcion que crea un nodo que se utiliza para llevar informacion sobre el AST.
 * @param info cadena que lleva informacion del nodo.
 * @param type tipo de dato que llevara el nodo.
 * @return Nuevo nodo creado.
 */
node* create_node(char* info, VarType type, int line, int column){
    node* root = new_node(NODE_INFO, line, column);
    root->info->NODE_INFO.type = type;
    root->info->NODE_INFO.info = info;

    return root;
}

/**
 * Funcion que crea un nodo de un tipo en particular.
 * @param type Tipo de nodo.
 * @return Nuevo nodo creado.
 */
node* new_node(NodeType type, int line, int column){
    node* root = malloc(sizeof(node));
    root->info = malloc(sizeof(union type));
    root->type = type;
    root->left = NULL;
    root->right = NULL;
    root->line = line;
    root->column = column;

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
void print_node(node *root, int level, FILE* parser_out) {
    if (!root)
        return;
    switch (root->type) {
        case NODE_NUM:
            fprintf(parser_out, "%d\n", root->info->INT.value);
            break;
        case NODE_BOOL:
            fprintf(parser_out, "%s\n", root->info->BOOL.value ? "true" : "false");
            break;
        case NODE_OP:
            switch (root->info->OP.name) {
            case OP_GT: fprintf(parser_out, ">\n"); break;
            case OP_LT: fprintf(parser_out, "<\n"); break;                                
            case OP_PLUS: fprintf(parser_out, "+ offset %d\n",  root->info->OP.offset); break;
            case OP_SUB: fprintf(parser_out, "-\n"); break;
            case OP_MULT: fprintf(parser_out, "*\n"); break;
            case OP_DIV: fprintf(parser_out, "/\n"); break;
            case OP_REST: fprintf(parser_out, "%%\n"); break;
            case OP_MINUS: fprintf(parser_out, "-\n"); break;                
            case OP_ASSIGN: fprintf(parser_out, "=\n"); break; 
            case OP_EQUALS: fprintf(parser_out, "==\n"); break;                           
            case OP_NOT: fprintf(parser_out, "!\n"); break;                
            case OP_OR: fprintf(parser_out, "||\n"); break;
            case OP_AND: fprintf(parser_out, "&&\n"); break;
            default: fprintf(parser_out, "OP?\n"); break;
            }
            break;
        case NODE_RET: fprintf(parser_out, "ret \n"); break;
        case NODE_DECL:
            switch(root->info->ID.type){
                case TYPE_INT: fprintf(parser_out, "int "); break;
                case TYPE_BOOL: fprintf(parser_out, "bool "); break;
                case NONE: fprintf(parser_out, "none "); break;
            }
            fprintf(parser_out, "%s offset %d\n", root->info->ID.name ? root->info->ID.name : "NULL", root->info->ID.offset);
            break;
        case NODE_ID_USE:
            fprintf(parser_out, "%s offset %d\n", root->info->ID.name ? root->info->ID.name : "NULL", root->info->ID.offset);
            break;
        case NODE_DECL_METH:
            if (root->info->METH_DECL.is_extern  == 1) {
                fprintf(parser_out, "extern ");
            }
            switch(root->info->METH_DECL.returnType) {
                case TYPE_INT:
                    fprintf(parser_out, "int ");
                    fprintf(parser_out, "%s", root->info->METH_DECL.name);
                    break;
                case TYPE_BOOL:
                    fprintf(parser_out, "bool ");
                    fprintf(parser_out, "%s", root->info->METH_DECL.name);
                    break; 
                case NONE:
                    fprintf(parser_out, "void ");
                    fprintf(parser_out, "%s", root->info->METH_DECL.name);
                    break;
            }
            fprintf(parser_out, "%s size: %d \n ", list_to_string(root->info->METH_DECL.f_params), root->info->METH_DECL.f_params ? root->info->METH_DECL.f_params->size : 0 );      
            break;
        case NODE_CALL_METH:
            fprintf(parser_out, " %s",root->info->METH_CALL.name);
            print_c_params(root->info->METH_CALL.c_params, parser_out);
            fprintf(parser_out, " size: %d", root->info->METH_CALL.c_params ? root->info->METH_CALL.c_params->size : 0 );
            fprintf(parser_out, "\n");
            break;
        case NODE_IF_ELSE:
            fprintf(parser_out, "if \n");
            save_ast(root->info->IF_ELSE.expr, level + 1, parser_out);
            save_ast(root->info->IF_ELSE.if_block, level+1, parser_out);
            save_ast(root->info->IF_ELSE.else_block, level+1, parser_out);
            break;
        case NODE_WHILE:
            fprintf(parser_out, "while \n");
            save_ast(root->info->WHILE.expr, level+1, parser_out);
            save_ast(root->info->WHILE.block, level+1, parser_out);
            break;
        case NODE_PYC: printf("PYC\n"); break;
        case NODE_INFO:
            fprintf(parser_out, "%s\n", root->info->NODE_INFO.info ? root->info->NODE_INFO.info : "NULL");
            break;
        case NODE_BLOCK: 
            fprintf(parser_out, "block \n");
            break;
        default:
            fprintf(parser_out, "UNKNOWN NODE\n");
            break;
        }
}

/**
 * Funcion de entrada que se encarga de imprimir el arbol.
 * @param root Tipo de nodo.
 * @param level Indica el indice del espaciado para imprimir correctamente.
 */
void save_ast(node *root, int level, FILE* parser_out) {
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++){
        fprintf(parser_out,"   ");
    }

    print_node(root, level, parser_out);
    save_ast(root->left, level + 1, parser_out);
    save_ast(root->right, level + 1, parser_out);
}


