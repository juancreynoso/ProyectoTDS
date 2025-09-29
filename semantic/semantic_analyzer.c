#include "semantic_analyzer.h"
#include <string.h>
//#include "type_checker.h"
//#include "expr_solver.h"

int exists_main = 0;
static VarType current_return_type = NONE;

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

        if (exists_main == 0){
            printf("Error. El programa debe incluir un metodo MAIN\n");
            exit(EXIT_FAILURE);
        }

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

            if (strcmp(s.info->METH_DECL.name, "main") == 0) {
                if(exists_main == 1){
                    printf("\nError. No se puede declarar mas de un metodo MAIN \n");
                    exit(EXIT_FAILURE);
                }
                exists_main = 1;
            }

            printf("Se declara nuevo método: %s\n", s.info->METH_DECL.name);
            insert_symbol(&(stack->top->data), s, root->type);
            
            VarType previous_return_type = current_return_type;
            current_return_type = root->info->METH_DECL.returnType;
            
            semantic_analysis_recursive(root->left, stack, table, root);
            semantic_analysis_recursive(root->right, stack, table, root);
            
            current_return_type = previous_return_type;
            break;
        }

        case NODE_IF_ELSE: {
            VarType cond_type = get_expression_type(root->info->IF_ELSE.expr, stack);
            if (cond_type != TYPE_BOOL) {
                printf("Error: la condición del IF debe ser bool\n");
                exit(EXIT_FAILURE);
            }
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
            VarType cond_type = get_expression_type(root->info->WHILE.expr, stack);
            if (cond_type != TYPE_BOOL) {
                printf("Error: la condición del WHILE debe ser bool\n");
                exit(EXIT_FAILURE);
            }
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
            union type* info = search_symbol(stack, root->info->ID.name, NODE_DECL); 
            if (!info) {
                printf("Error: Variable '%s' no declarada\n", root->info->ID.name);
                exit(EXIT_FAILURE);
            }
            root->info = info;
            break;
        }

        case NODE_CALL_METH: {
            
            union type* method = search_symbol(stack, root->info->METH_CALL.name, NODE_DECL_METH);
            if (!method) {
                printf("Error: Método '%s' no declarado\n", root->info->METH_CALL.name);
                exit(EXIT_FAILURE);
            }
            
            if (root->info->METH_CALL.c_params != NULL) {
                if (!verify_method_params(method->METH_DECL.f_params, root->info->METH_CALL.c_params, stack, root->info->METH_CALL.name)) {
                    exit(EXIT_FAILURE);
                }           
            } else if(method->METH_DECL.f_params != NULL){
                if (method->METH_DECL.f_params->size > 0) {
                    printf("Error: método '%s' requiere %d parámetro/s \n", root->info->METH_CALL.name, method->METH_DECL.f_params->size);
                    exit(EXIT_FAILURE);
                }
            } 
            root->info->METH_CALL.returnType = method->METH_DECL.returnType;
            printf("Call method: %s %s\n", type_to_string(root->info->METH_CALL.returnType),  root->info->METH_CALL.name);
            break;
        }

        case NODE_OP: {
            switch(root->info->OP.name) {
                case OP_ASSIGN:{
                    semantic_analysis_recursive(root->left, stack, table, NULL);
                    semantic_analysis_recursive(root->right, stack, table, NULL);
                    get_expression_type(root, stack);
                    break; 
                }
                default: {
                    break;
                }
            }
            break;
        }

        case NODE_RET: {
            semantic_analysis_recursive(root->left, stack, table, NULL);
            VarType return_type = get_expression_type(root->left, stack);
            if (return_type != current_return_type) {
                printf("Error: tipo de retorno incompatible. Esperado '%s', Recibido '%s'\n", 
                    type_to_string(current_return_type), type_to_string(return_type));
                exit(EXIT_FAILURE);
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
    if (!f_params || !f_params->head) return;
    
    Node_P_List* cursor = f_params->head;
    while (cursor != NULL) {
        union type* param_info = malloc(sizeof(union type));
        param_info->ID.name = strdup(cursor->p.name);
        param_info->ID.type = cursor->p.type;
        param_info->ID.value.num = 0; 
        
        symbol s;
        s.info = param_info;
        printf("Se declara parámetro formal: %s\n", cursor->p.name);
        insert_symbol(&(stack->top->data), s, NODE_DECL);
        
        cursor = cursor->next;
    }
}


/**
 * Verifica que los parámetros actuales de una llamada a método estén declarados.
 * Recorre la lista de argumentos actuales y valida que cada variable utilizada exista.
 * Para expresiones complejas, delega el análisis a la función recursiva principal.
 * @param actual_params Lista de parámetros actuales de la llamada
 * @param formal_params Lista de parámetros formales del método
 * @param stack Pila de scopes para búsqueda de símbolos
 * @param table Tabla del scope actual
 */
int verify_method_params(Formal_P_List* formal_params, Current_P_List* actual_params, tables_stack* stack, char* method_name) {
    if (!formal_params) {
        printf("Error: %s no posee parametros en su definicion\n", method_name);
        return 0;
    }
    // Verificar cantidad
    if (formal_params->size != actual_params->size) {
        printf("Error: método '%s' espera %d parámetros, se pasaron %d\n", method_name, formal_params->size, actual_params->size);
        return 0;
    }
    
    // Verificar compatibilidad 1 a 1
    Node_P_List* formal_cursor = formal_params->head;
    Node_C_List* actual_cursor = actual_params->head;

    for (int i = 0; i < formal_params->size; i++) {
        VarType formal_p = formal_cursor->p.type;
        VarType actual_p = get_expression_type(actual_cursor->p, stack);

        if (formal_p != actual_p) {
            printf("Error en %s: tipos de parametros incompatibles, Esperado '%s', Recibido: '%s' \n", method_name,
                type_to_string(formal_p), type_to_string(actual_p));
            exit(EXIT_FAILURE);
        }
        formal_cursor = formal_cursor->next;
        actual_cursor = actual_cursor->next;
    }
    return 1;
}

VarType get_expression_type(node* root, tables_stack* stack) {
    if (root == NULL) {
        return NONE;
    }

    if (root->left == NULL && root->right == NULL) {
        switch(root->type) {
            case NODE_NUM:
                return TYPE_INT;
                break;
            case NODE_BOOL:
                return TYPE_BOOL;
                break;
            case NODE_ID_USE: {
                union type* var_info = search_symbol(stack, root->info->ID.name, NODE_DECL);
                if (!var_info) {
                    printf("Error: variable '%s' no encontrada para verificación de tipos\n", root->info->ID.name);
                    exit(EXIT_FAILURE);
                }
                return var_info->ID.type;
            }
            case NODE_DECL:
                return root->info->ID.type;
                break;
            case NODE_CALL_METH:
                return root->info->METH_CALL.returnType;
                break;
            default:
                break;
        }
    }

    if (root->left != NULL && root->right == NULL) {
        if (root->type == NODE_OP) {
            VarType leftType = get_expression_type(root->left, stack);

            switch (root->info->OP.name) {
                case OP_NOT:
                    if (leftType != TYPE_BOOL) {
                            printf("Error de tipos. Requiere tipo booleano\n");
                            exit(EXIT_FAILURE);
                    }
                    return root->info->OP.type = leftType;
                    break;
                case OP_MINUS:
                    if (leftType != TYPE_INT) {
                            printf("Error de tipos. Requiere tipo entero\n");
                            exit(EXIT_FAILURE);
                    }
                    return root->info->OP.type = leftType;
                    break;
                default:
                    break;
            }
        } else {
            return get_expression_type(root->left, stack);
        }
    }

    if (root->left == NULL && root->right != NULL) {
        return get_expression_type(root->right, stack);
    }

    if (root->left != NULL && root->right != NULL) {
        VarType leftType = get_expression_type(root->left, stack);
        VarType rightType = get_expression_type(root->right, stack);

        switch (root->info->OP.name) {
            case OP_PLUS:
            case OP_SUB:
            case OP_DIV:
            case OP_MULT:
            case OP_REST:
                if(leftType != TYPE_INT || rightType != TYPE_INT) {
                    printf("Error de tipos. Requiere tipo entero\n");
                    exit(EXIT_FAILURE); 
                }

                root->info->OP.type = leftType;
                return leftType;
                break;         

            case OP_ASSIGN:
                if(leftType != rightType) {
                    printf("Error: tipos incompatibles, Esperado '%s', Recibido: '%s' \n", 
                type_to_string(leftType), type_to_string(rightType));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;
                break;   
            case OP_OR:
            case OP_AND:

                if(leftType != TYPE_BOOL || rightType!= TYPE_BOOL) {
                    printf("Error de tipos. Requiere tipo booleano\n");
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;
                break;
            case OP_EQUALS:
            case OP_GT:
            case OP_LT:
                return TYPE_BOOL;
            default:
                printf("Operacion desconocida.\n");
                return leftType;
                break;
        }
        
    }
    printf("Error\n");
    exit(EXIT_FAILURE); 
}

/*
 * Realiza el chequeo de tipos entre el retorno y el perfil de la funcion
 */
void check_return_type(node* root, VarType f_returnType, tables_stack* stack) {
    if (root == NULL ) {
        return ;
    }

    if (root->type == NODE_RET) {
        VarType retType = get_expression_type(root, stack);
        if (retType !=  f_returnType) {
            printf("ERROR - tipo de retorno incompatible\n");
            printf("%s f \n", type_to_string(f_returnType));
            printf("return expr: %s\n", type_to_string(retType));
            exit(EXIT_FAILURE);
        }
    } else {
        check_return_type(root->left, f_returnType, stack);
        check_return_type(root->right, f_returnType, stack);
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