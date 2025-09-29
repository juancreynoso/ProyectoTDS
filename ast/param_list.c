#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "param_list.h"
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
void print_expr(node* root){
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
                    print_expr(root->left);
                    printf(" > ");
                    print_expr(root->right);
                    break;
                case OP_LT:
                    print_expr(root->left);
                    printf(" < "); 
                    print_expr(root->right); 
                    break;                               
                case OP_PLUS: 
                    print_expr(root->left);
                    printf(" + "); 
                    print_expr(root->right);
                    break;
                case OP_SUB: 
                    print_expr(root->left);
                    printf(" - "); 
                    print_expr(root->right);
                    break;
                case OP_MULT: 
                    print_expr(root->left);
                    printf(" * "); 
                    print_expr(root->right);
                    break;
                case OP_DIV: 
                    print_expr(root->left);
                    printf(" / "); 
                    print_expr(root->right);
                    break;
                case OP_REST: 
                    print_expr(root->left);
                    printf("%%\n"); 
                    print_expr(root->right);
                    break;
                case OP_MINUS: 
                    printf(" - "); 
                    print_expr(root->left);
                    break;                 
                case OP_EQUALS: 
                    print_expr(root->left);
                    printf(" == ");
                    print_expr(root->right);  
                    break;                           
                case OP_NOT: 
                    printf("!"); 
                    print_expr(root->left); 
                    break;                
                case OP_OR:
                    print_expr(root->left); 
                    printf(" || "); 
                    print_expr(root->right); 
                    break;
                case OP_AND: 
                    print_expr(root->left);
                    printf(" && "); 
                    print_expr(root->right);
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
 * Funcion que imprime una lista de parametros reales
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
        print_expr(cursor->p);
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