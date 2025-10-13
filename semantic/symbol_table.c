#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int exists_main = 0;


/**
 * Crea una pila vacía para gestionar los scopes de símbolos.
 * Inicializa la estructura de datos que mantiene los diferentes niveles de alcance.
 * @return Puntero a la nueva pila de tablas de símbolos
 */
tables_stack* create_stack() {
    tables_stack* stack = malloc(sizeof(tables_stack));
    stack->top = NULL;
    return stack;
}

/**
 * Inserta un nuevo scope en el tope de la pila (operación push).
 * Cada push corresponde a entrar en un nuevo bloque de código con su propio alcance.
 * @param stack: Pila de scopes donde insertar
 * @param table: Nueva tabla de símbolos que representa el scope
 */
void push(tables_stack* stack, symbol_table* table) {
    node_s* new_node = malloc(sizeof(node_s));
    new_node->data = table;      // este scope (tabla de símbolos)
    new_node->next = stack->top; // apilar sobre el actual
    stack->top = new_node;
}

/**
 * Remueve el scope del tope de la pila (operación pop).
 * Se utiliza al salir de un bloque de código, eliminando las variables locales de ese scope.
 * @param stack Pila de scopes de donde remover
 * @return Tabla de símbolos del scope removido
 */
symbol_table* pop(tables_stack* stack) {
    if (stack->top == NULL) return NULL;

    node_s* temp = stack->top;
    symbol_table* data = temp->data;
    stack->top = temp->next;
    free(temp);
    return data;
}

/**
 * Función principal que inicia el análisis semántico del AST.
 * Crea la pila de scopes, inicializa el scope global y comienza el análisis recursivo.
 * @param root Nodo raíz del AST a analizar
 * @return Pila de tablas de símbolos construida durante el análisis
 */
tables_stack* analyze_semantics(node* root, FILE* semantic_out) {
        tables_stack *stack = create_stack();
        symbol_table *table = NULL;
        push(stack, table);

        fprintf(semantic_out, ">>> Se crea scope global\n");
        semantic_analysis_recursive(root, stack, table, NULL, semantic_out);
        fprintf(semantic_out, "<<< Se cierra scope global\n");

        if (exists_main == 0){
            printf("Error. El programa debe incluir un metodo MAIN\n");
            exit(EXIT_FAILURE);
        }

        return stack;
}

/**
 * Inserta un símbolo en la tabla de símbolos del scope actual.
 * Verifica que no este duplicado en el mismo scope antes de insertar.
 * Maneja tanto variables como métodos según el tipo de nodo.
 * @param table Referencia a la tabla de símbolos donde insertar
 * @param s Símbolo a insertar
 * @param node_type Tipo de nodo (NODE_DECL, NODE_DECL_METH, etc.)
 */
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

/**
 * Busca un símbolo por nombre en toda la pila de scopes.
 * Recorre desde el scope más interno hacia el más externo hasta encontrar el símbolo.
 * Implementa las reglas de scoping léxico del lenguaje.
 * @param stack Pila de scopes donde buscar
 * @param name Nombre del símbolo a buscar
 * @return Puntero a la información del símbolo encontrado, NULL si no existe
 */
union type* search_symbol(tables_stack *stack, char* name, NodeType type) {
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
                default:
                    break;
            }
            
            if (strcmp(symbol_name, name) == 0 && cursor->nodeType == type) {
                return cursor->s.info;
            }
            cursor = cursor->next;
        }
    }
    return NULL;
}

/**
 * Busca un símbolo por nombre únicamente en una tabla específica (un solo scope).
 * Utilizada para verificar duplicados en el mismo scope antes de insertar.
 * @param table Tabla de símbolos específica donde buscar
 * @param name Nombre del símbolo a buscar
 * @return Puntero a la información del símbolo encontrado, NULL si no existe en esa tabla
 */
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

/**
 * Imprime el contenido completo de una tabla de símbolos.
 * Muestra información detallada de cada símbolo (variables y métodos) para debugging.
 * @param table Tabla de símbolos a imprimir
 */
void print_symbol_table(symbol_table *table, FILE* semantic_out) {
    if (table == NULL) {
        fprintf(semantic_out, "(scope vacío)\n");
        return;
    }
    
    symbol_table *cursor = table;
    fprintf(semantic_out, "\n---- Inicio Tabla de Simbolos ----\n");
    
    while(cursor != NULL) {
        switch(cursor->nodeType) {
            case NODE_DECL:
            case NODE_ID_USE:
                fprintf(semantic_out, "VARIABLE: %s\n", cursor->s.info->ID.name);
                fprintf(semantic_out, "TYPE: %s\n", cursor->s.info->ID.type == TYPE_INT ? "INT" : "BOOL");
                break;
                
            case NODE_DECL_METH:
                fprintf(semantic_out, "METHOD: %s\n", cursor->s.info->METH_DECL.name);
                fprintf(semantic_out, "RETURN: %s\n", 
                       cursor->s.info->METH_DECL.returnType == TYPE_INT ? "INT" : 
                       cursor->s.info->METH_DECL.returnType == TYPE_BOOL ? "BOOL" : "VOID");
                break;
            default:
                break;;
        }
        
        if (cursor->next != NULL) {
            fprintf(semantic_out, "-----\n");
        }
        cursor = cursor->next;
    }
    fprintf(semantic_out, "\n---- Fin Tabla de Simbolos ----\n");
}

