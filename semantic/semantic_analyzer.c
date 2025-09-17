#include "semantic_analyzer.h"
#include "type_checker.h"
#include "expr_solver.h"

/*
  Dado un arbol como parametro, crea la tabla de simbolos
  utilizando la funcion auxiliar
*/
tables_stack* analyze_semantics(node* root) {
        tables_stack *stack = create_stack();
        symbol_table *table = NULL;
        push(stack, table);
        semantic_analysis_recursive(root, stack, table);
        return stack;
}

/*
  Implementa la construccion de la tabla de simbolos con una pila.
  Cada nodo de la pila es un ambiente
*/
void semantic_analysis_recursive(node* root, tables_stack* stack, symbol_table* table) {
    if (!root) return;

    switch (root->type) {
        case NODE_DECL_METH:
        case NODE_IF_ELSE:
        case NODE_WHILE: {
            // Nuevo scope
            symbol_table* new_table = NULL;
            push(stack, new_table);
            printf("Nuevo scope\n");

            semantic_analysis_recursive(root->left, stack, stack->top->data);
            semantic_analysis_recursive(root->right, stack, stack->top->data);

            pop(stack);
            printf("Cierra scope\n");
            break;
        }

        case NODE_DECL: {
            symbol s;
            s.info = root->info;
            insert_symbol(&(stack->top->data), s, root->type); // Se declara una variable en la tabla del scope actual (tope de la pila)
            break;
        }

        case NODE_ID_USE: {
            // Se verifica la declaracion de un simbolo en toda la pila para ser usado
            union type* info = search_symbol(stack, root->info->ID.name); 
            if (!info) {
                printf("Error: Variable '%s' no declarado\n", root->info->ID.name);
                exit(EXIT_FAILURE);
            }
            root->info = info; // enlazo nodo con la info real
            break;
        }

        case NODE_OP: { // Falta completar
            semantic_analysis_recursive(root->left, stack, table);
            semantic_analysis_recursive(root->right, stack, table);
            break;
        }

        default:
            semantic_analysis_recursive(root->left, stack, table);
            semantic_analysis_recursive(root->right, stack, table);
            break;
    }
}

void insert_symbol(symbol_table **table, symbol s, NodeType nodeType) {
    if (search_in_table(*table, s.info->ID.name) == NULL) {
        symbol_table *aux = malloc(sizeof(symbol_table));
        aux->s.info = s.info;
        aux->nodeType = nodeType;
        aux->next = *table;
        *table = aux;
    } else {
        printf("error, variable already exists\n");
        exit(EXIT_FAILURE);
    }
}

union type* search_symbol(tables_stack *stack, char* name) {
    for (node_s* scope = stack->top; scope != NULL; scope = scope->next) {
        symbol_table* cursor = scope->data;
        while (cursor != NULL) {
            if (strcmp(cursor->s.info->ID.name, name) == 0) {
                return cursor->s.info; // encontrado en este ambiente
            }
            cursor = cursor->next;
        }
    }
    return NULL; // no encontrado en ningún ambiente
}

union type* search_in_table(symbol_table* table, char* name) {
    if (table == NULL) {
        return NULL;
    }
    symbol_table *cursor = table;
    while(cursor != NULL) {
        if (strcmp(cursor->s.info->ID.name, name) == 0) {
            return cursor->s.info;
        }
        cursor = cursor->next;
    }
    return NULL;
}

// Crear pila vacía
tables_stack* create_stack() {
    tables_stack* stack = malloc(sizeof(tables_stack));
    stack->top = NULL;
    return stack;
}

// Push (insertar al inicio)
void push(tables_stack* stack, symbol_table* table) {
    node_s* new_node = malloc(sizeof(node_s));
    new_node->data = table;      // este scope (tabla de símbolos)
    new_node->next = stack->top; // apilar sobre el actual
    stack->top = new_node;
}

// Pop (sacar del inicio)
symbol_table* pop(tables_stack* stack) {
    if (stack->top == NULL) return NULL;

    node_s* temp = stack->top;
    symbol_table* data = temp->data;
    stack->top = temp->next;
    free(temp);
    return data;
}