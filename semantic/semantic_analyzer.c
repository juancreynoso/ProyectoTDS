#include "semantic_analyzer.h"
//#include "type_checker.h"
//#include "expr_solver.h"

/**
 * Función principal que inicia el análisis semántico del AST.
 * Crea la pila de scopes, inicializa el scope global y comienza el análisis recursivo.
 * @param root Nodo raíz del AST a analizar
 * @return Pila de tablas de símbolos construida durante el análisis
 */
tables_stack* analyze_semantics(node* root) {
        tables_stack *stack = create_stack();
        symbol_table *table = NULL;
        push(stack, table);
        printf(">>> Se crea scope global\n");
        semantic_analysis_recursive(root, stack, table, NULL);
        printf("<<< Se cierra scope global\n");
        return stack;
}

/**
 * Función recursiva que implementa el análisis semántico del AST.
 * Maneja la construcción de la tabla de símbolos mediante una pila de scopes.
 * Cada tipo de nodo tiene su tratamiento específico según las reglas semánticas.
 * @param root Nodo actual del AST a procesar
 * @param stack Pila de tablas de símbolos (scopes anidados)
 * @param table Tabla de símbolos del scope actual
 * @param parent Nodo padre del nodo actual (para contexto)
 */
void semantic_analysis_recursive(node* root, tables_stack* stack, symbol_table* table, node* parent) {
    if (!root) return;

    switch (root->type) {
        case NODE_DECL_METH: {
            symbol s;
            s.info = root->info;
            printf("Se declara nuevo método: %s\n", s.info->METH_DECL.name);
            insert_symbol(&(stack->top->data), s, root->type);
            
            semantic_analysis_recursive(root->left, stack, table, root);
            semantic_analysis_recursive(root->right, stack, table, root);
            break;
        }

        case NODE_IF_ELSE: {
            semantic_analysis_recursive(root->info->IF_ELSE.expr, stack, table, NULL);
            
            if (root->info->IF_ELSE.if_block) {
                semantic_analysis_recursive(root->info->IF_ELSE.if_block, stack, table, root);
            }
            if (root->info->IF_ELSE.else_block) {
                semantic_analysis_recursive(root->info->IF_ELSE.else_block, stack, table, root);
            }
            break;
        }

        case NODE_WHILE: {
            semantic_analysis_recursive(root->info->WHILE.expr, stack, table, NULL);
            semantic_analysis_recursive(root->info->WHILE.block, stack, table, root);
            break;
        }

        case NODE_BLOCK: {
            symbol_table* new_table = NULL;
            push(stack, new_table);
            
            if (parent) {
                switch(parent->type) {
                    case NODE_DECL_METH:
                        printf(">>> Nuevo scope (Método: %s)\n", parent->info->METH_DECL.name);
                        // Agregar parámetros formales al scope del método
                        if (parent->info->METH_DECL.f_params) {
                            add_formal_params_to_scope(stack, parent->info->METH_DECL.f_params);
                        }
                        break;
                    case NODE_WHILE:
                        printf(">>> Nuevo scope (WHILE)\n");
                        break;
                    case NODE_IF_ELSE:
                        if (root == parent->info->IF_ELSE.if_block) {
                            printf(">>> Nuevo scope (IF)\n");
                        } else if (root == parent->info->IF_ELSE.else_block) {
                            printf(">>> Nuevo scope (ELSE)\n");
                        } else {
                            printf(">>> Nuevo scope (IF/ELSE)\n");
                        }
                        break;
                    default:
                        printf(">>> Nuevo scope (BLOCK)\n");
                        break;
                }
            } else {
                printf(">>> Nuevo scope (BLOCK - Aislado)\n");
            }
            
            semantic_analysis_recursive(root->left, stack, stack->top->data, NULL);
            semantic_analysis_recursive(root->right, stack, stack->top->data, NULL);
            
            printf("Scope del bloque antes de cerrarlo:\n");
            print_symbol_table(stack->top->data);
            
            pop(stack);
            printf("<<< Cierra scope (BLOCK)\n");
            break;
        }

        case NODE_DECL: {
            symbol s;
            s.info = root->info;
            printf("Se declara variable: %s\n", s.info->ID.name);
            insert_symbol(&(stack->top->data), s, root->type);
            semantic_analysis_recursive(root->right, stack, table, NULL);
            break;
        }

        case NODE_ID_USE: {
            union type* info = search_symbol(stack, root->info->ID.name); 
            if (!info) {
                printf("Error: Variable '%s' no declarada\n", root->info->ID.name);
                exit(EXIT_FAILURE);
            }
            root->info = info;
            break;
        }

        case NODE_CALL_METH: {
            union type* method = search_symbol(stack, root->info->METH_CALL.name);
            if (!method) {
                printf("Error: Método '%s' no declarado\n", root->info->METH_CALL.name);
                exit(EXIT_FAILURE);
            }
            printf("Call method: %s\n", root->info->METH_CALL.name);
            
            if (root->info->METH_CALL.c_params != NULL) {
                check_current_params(root->info->METH_CALL.c_params, stack, table);
            }
            break;
        }

        default:
            semantic_analysis_recursive(root->left, stack, table, NULL);
            semantic_analysis_recursive(root->right, stack, table, NULL);
            break;
    }
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
                default:
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
void print_symbol_table(symbol_table *table) {
    if (table == NULL) {
        printf("(scope vacío)\n");
        return;
    }
    
    symbol_table *cursor = table;
    printf("\n---- Inicio Tabla de Simbolos ----\n");
    
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
            default:
                break;;
        }
        
        if (cursor->next != NULL) {
            printf("-----\n");
        }
        cursor = cursor->next;
    }
    printf("\n---- Fin Tabla de Simbolos ----\n");
}

/**
 * Agrega los parámetros formales de un método al scope actual.
 * Recorre la lista de parámetros formales y los inserta como símbolos en el scope del método.
 * Cada parámetro se trata como una declaración de variable local al método.
 * @param stack Pila de scopes (para insertar en el scope actual)
 * @param f_params Lista de parámetros formales del método
 */
void add_formal_params_to_scope(tables_stack* stack, Formal_P_List* f_params) {
    Formal_P_List* cursor = f_params;
    while (cursor->head != NULL) {
        union type* param_info = malloc(sizeof(union type));
        param_info->ID.name = strdup(cursor->head->p.name); // se copia el nombre
        param_info->ID.type = cursor->head->p.type;
        param_info->ID.value.num = 0; 
        
        // se agrega al scope actual
        symbol s;
        s.info = param_info;
        printf("Se declara parámetro formal: %s \n", cursor->head->p.name);
        insert_symbol(&(stack->top->data), s, NODE_DECL);
        // Busco el proximo parametro
        cursor->head = cursor->head->next;
    }
}

/**
 * Verifica que los parámetros actuales de una llamada a método estén declarados.
 * Recorre la lista de argumentos actuales y valida que cada variable utilizada exista.
 * Para expresiones complejas, delega el análisis a la función recursiva principal.
 * @param c_params Lista de parámetros actuales de la llamada
 * @param stack Pila de scopes para búsqueda de símbolos
 * @param table Tabla del scope actual
 */
void check_current_params(Current_P_List* c_params, tables_stack* stack, symbol_table* table) {
    Current_P_List* cursor = c_params;
    printf("Checking current parameters...\n");
    while (cursor->head != NULL) {
        if (cursor->head->p->type == NODE_ID_USE) {
            union type* param = search_symbol(stack, cursor->head->p->info->ID.name);
            if (!param) {
                printf("Error: Parámetro '%s' no declarado\n", cursor->head->p->info->ID.name);
                exit(EXIT_FAILURE);
            }
            printf("Parámetro '%s' encontrado\n", cursor->head->p->info->ID.name);
        } else {
            semantic_analysis_recursive(cursor->head->p, stack, table, NULL);
        }
        cursor->head = cursor->head->next;
    }
}

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