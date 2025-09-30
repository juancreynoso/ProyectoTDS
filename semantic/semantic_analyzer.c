#include "semantic_analyzer.h"
#include <string.h>

static VarType current_return_type = NONE;

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
                printf("Error [línea %d, columna %d]: la condición del IF debe ser bool\n", root->line, root->column);
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
                printf("Error [línea %d, columna %d]: la condición del WHILE debe ser bool\n", root->line, root->column);
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
                printf("Error [línea %d, columna %d]: Variable '%s' no declarada\n", 
               root->line, root->column, root->info->ID.name);
                exit(EXIT_FAILURE);
            }
            root->info = info;
            break;
        }

        case NODE_CALL_METH: {
            
            union type* method = search_symbol(stack, root->info->METH_CALL.name, NODE_DECL_METH);
            if (!method) {
                printf("Error [línea %d, columna %d]: Método '%s' no declarado\n", root->line, root->column, root->info->METH_CALL.name);
                exit(EXIT_FAILURE);
            }
            
            if (root->info->METH_CALL.c_params != NULL) {
                if (!verify_method_params(method->METH_DECL.f_params, root->info->METH_CALL.c_params, stack, root->info->METH_CALL.name)) {
                    exit(EXIT_FAILURE);
                }           
            } else if(method->METH_DECL.f_params != NULL){
                if (method->METH_DECL.f_params->size > 0) {
                    printf("Error [línea %d, columna %d]: método '%s' requiere %d parámetro/s \n", 
                        root->line, root->column, root->info->METH_CALL.name, method->METH_DECL.f_params->size);
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
                printf("Error de tipo [línea %d, columna %d]: tipo de retorno incompatible. Esperado '%s', Recibido '%s'\n", 
                    root->line, root->column, type_to_string(current_return_type), type_to_string(return_type));
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
