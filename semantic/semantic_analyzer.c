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
        case NODE_DECL_METH: {
            symbol s;
            s.info = root->info;
            printf("Se declara nuevo método: %s\n", s.info->METH_DECL.name);
            insert_symbol(&(stack->top->data), s, root->type);
            
            // Nuevo scope del metodo
            symbol_table* new_table = NULL;
            push(stack, new_table);
            printf(">>> Nuevo scope (METH %s)\n", root->info->METH_DECL.name);

            // Se agregan los parametros al scope
            if (root->info->METH_DECL.f_params != NULL) {
                add_formal_params_to_scope(stack, root->info->METH_DECL.f_params);
            }
            semantic_analysis_recursive(root->left, stack, stack->top->data);
            semantic_analysis_recursive(root->right, stack, stack->top->data);
            
            printf("Scope del método antes de cerrarlo:\n");
            print_symbol_table(stack->top->data);

            pop(stack);
            printf("<<< Cierra scope (METH)\n");
            break;
        }
        case NODE_IF_ELSE:
        case NODE_WHILE: {
            // Nuevo scope
            symbol_table* new_table = NULL;
            push(stack, new_table);
            printf(">>> Nuevo scope (WHILE/IF)\n");

            semantic_analysis_recursive(root->left, stack, stack->top->data);
            semantic_analysis_recursive(root->right, stack, stack->top->data);

            printf("Contenido del scope antes de cerrarlo:\n");
            print_symbol_table(stack->top->data);

            pop(stack);
            printf("<<< Cierra scope ((WHILE/IF))\n");
            break;
        }

        case NODE_DECL: {
            // Declaracion en el mismo scope
            symbol s;
            s.info = root->info;
            printf("Se declara nuevo simbolo: %s \n", s.info->ID.name);
            insert_symbol(&(stack->top->data), s, root->type); // Se declara una variable en la tabla del scope actual (tope de la pila)
            break;
        }

        case NODE_ID_USE: {
            // Se verifica la declaracion de un simbolo en toda la pila para ser usado
            union type* info = search_symbol(stack, root->info->ID.name); 
            if (!info) {
                printf("Error: Variable '%s' no declarada\n", root->info->ID.name);
                exit(EXIT_FAILURE);
            }
            root->info = info; // enlazo nodo con la info real
            break;
        }

        case NODE_CALL_METH: {
            union type* method = search_symbol(stack, root->info->METH_CALL.name);
            if (!method) {
                printf("Error: Método '%s' no declarado\n", root->info->METH_CALL.name);
                exit(EXIT_FAILURE);
            }
            printf("Llamando método: %s\n", root->info->METH_CALL.name);
            
            if (root->info->METH_CALL.c_params != NULL) {
                check_current_params(root->info->METH_CALL.c_params, stack, table);
            }
            
            break;
        }

        case NODE_OP: 
        default:
            semantic_analysis_recursive(root->left, stack, table);
            semantic_analysis_recursive(root->right, stack, table);
            break;
    }
}

void insert_symbol(symbol_table **table, symbol s, NodeType node_type) {
    char* name;
    switch(node_type) {
        case NODE_DECL:
        case NODE_ID_USE:
            // caso guardar decl var en scope
            name = s.info->ID.name;
            break;
        case NODE_DECL_METH:
            // caso guardar decl metodo en scope
            name = s.info->METH_DECL.name;
            break;
        default:
            printf("Error: tipo de nodo no soportado para inserción\n");
            return;
    }
    
    if (search_in_table(*table, name) == NULL) {
        symbol_table *aux = malloc(sizeof(symbol_table));
        aux->s.info = s.info;
        aux->nodeType = node_type;
        aux->next = *table;
        *table = aux;
    } else {
        printf("Error: '%s' ya declarado en este scope\n", name);
        exit(EXIT_FAILURE);
    }
}

union type* search_symbol(tables_stack *stack, char* name) {
    for (node_s* scope = stack->top; scope != NULL; scope = scope->next) {
        symbol_table* cursor = scope->data;
        while (cursor != NULL) {
            char* symbol_name = NULL;
            
            switch(cursor->nodeType) {
                case NODE_DECL:
                case NODE_ID_USE:
                    symbol_name = cursor->s.info->ID.name;
                    break;
                case NODE_DECL_METH:
                    symbol_name = cursor->s.info->METH_DECL.name;
                    break;
            }
            
            if (strcmp(symbol_name, name) == 0) {
                return cursor->s.info;
            }
            cursor = cursor->next;
        }
    }
    return NULL;
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

void print_symbol_table(symbol_table *table) {
    if (table == NULL) {
        printf("(scope vacío)\n");
        return;
    }
    
    symbol_table *cursor = table;
    printf("\n----- INICIO TABLA DE SIMBOLOS -----\n");
    
    while(cursor != NULL) {
        switch(cursor->nodeType) {
            case NODE_DECL:
            case NODE_ID_USE:
                printf("VARIABLE: %s\n", cursor->s.info->ID.name);
                printf("TYPE: %s\n", cursor->s.info->ID.type == TYPE_INT ? "INT" : "BOOL");
                break;
                
            case NODE_DECL_METH:
                printf("METHOD: %s\n", cursor->s.info->METH_DECL.name);
                printf("RETURN: %s\n", 
                       cursor->s.info->METH_DECL.returnType == TYPE_INT ? "INT" : 
                       cursor->s.info->METH_DECL.returnType == TYPE_BOOL ? "BOOL" : "VOID");
                break;
        }
        
        if (cursor->next != NULL) {
            printf("-----\n");
        }
        cursor = cursor->next;
    }
    printf("\n----- FIN TABLA DE SIMBOLOS -----\n");
}

void add_formal_params_to_scope(tables_stack* stack, Formal_P_List* f_params) {
    Formal_P_List* cursor = f_params;
    while (cursor != NULL) {
        union type* param_info = malloc(sizeof(union type));
        param_info->ID.name = strdup(cursor->p.name); // se copia el nombre
        param_info->ID.type = cursor->p.type;
        param_info->ID.value.num = 0; 
        
        // se agrega al scope actual
        symbol s;
        s.info = param_info;
        printf("Se declara parámetro formal: %s \n", cursor->p.name);
        insert_symbol(&(stack->top->data), s, NODE_DECL);
        // Busco el proximo parametro
        cursor = cursor->next;
    }
}

void check_current_params(Current_P_List* c_params, tables_stack* stack, symbol_table* table) {
    Current_P_List* cursor = c_params;
    printf("Checking actual parameters\n");
    while (cursor != NULL) {
        if (cursor->p->type == NODE_ID_USE) {
            union type* param = search_symbol(stack, cursor->p->info->ID.name);
            if (!param) {
                printf("Error: Parámetro '%s' no declarado\n", cursor->p->info->ID.name);
                exit(EXIT_FAILURE);
            }
            printf("Parámetro '%s' encontrado\n", cursor->p->info->ID.name);
        } else {
            semantic_analysis_recursive(cursor->p, stack, table);
        }
        cursor = cursor->next;
    }
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