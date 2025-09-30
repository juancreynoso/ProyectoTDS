#include "semantic_analyzer.h"

/**
 * Analiza semánticamente una expresión y obtiene su tipo.
 *
 * Recorre el AST de la expresión (nodo 'root') de manera recursiva.
 * Para cada nodo:
 *  - Si es una hoja (NUM, BOOL, ID, llamada a método), devuelve el tipo.
 *  - Si es un operador unario (NOT, MINUS), verifica el tipo del operando
 *    y devuelve el tipo resultante.
 *  - Si es un operador binario (aritmético, lógico, comparación, asignación),
 *    verifica la compatibilidad de tipos entre operandos y devuelve el tipo
 *    resultante.
 *
 * @param root Nodo del AST que representa la expresión a analizar.
 * @param stack Pila de tablas de símbolos, para verificar identificadores.
 * @return Tipo resultante de la expresión (TYPE_INT, TYPE_BOOL).
 */
VarType get_expression_type(node* root, tables_stack* stack) {
    if (root == NULL) {
        return NONE;
    }

    if (root->left == NULL && root->right == NULL) {
        switch(root->type) {
            case NODE_NUM:
                return TYPE_INT;

            case NODE_BOOL:
                return TYPE_BOOL;

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

            case NODE_CALL_METH:
                return root->info->METH_CALL.returnType;
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
                            printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere tipo booleano\n", 
                                root->line, root->column, op_name(root->info->OP.name));
                            exit(EXIT_FAILURE);
                    }
                    return root->info->OP.type = leftType;

                case OP_MINUS:
                    if (leftType != TYPE_INT) {
                            printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere tipo entero\n", 
                                root->line, root->column, op_name(root->info->OP.name));
                            exit(EXIT_FAILURE);
                    }
                    return root->info->OP.type = leftType;

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
                    printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere tipo entero\n", 
                                root->line, root->column, op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }

                root->info->OP.type = leftType;
                return leftType;        

            case OP_ASSIGN:
                if(leftType != rightType) {
                    printf("Error de tipo [línea %d, columna %d]: tipos incompatibles en asignación, Esperado '%s', Recibido: '%s' \n", 
                        root->line, root->column, type_to_string(leftType), type_to_string(rightType));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;

            case OP_OR:
            case OP_AND:

                if(leftType != TYPE_BOOL || rightType!= TYPE_BOOL) {
                    printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere tipo booleano\n", 
                        root->line, root->column, op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;

            case OP_EQUALS:
                if (!((leftType == TYPE_BOOL && rightType == TYPE_BOOL) || (leftType == TYPE_INT && rightType == TYPE_INT))) {
                    printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere operandos del mismo tipo\n", 
                        root->line, root->column, op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = TYPE_BOOL;
                return TYPE_BOOL;

            case OP_GT:
            case OP_LT:
                if(leftType != TYPE_INT || rightType != TYPE_INT) {
                    printf("Error de tipo [línea %d, columna %d]: Operación '%s' requiere tipo entero\n", 
                        root->line, root->column, op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = TYPE_BOOL;
                return TYPE_BOOL;

            default:
                printf("Operacion desconocida.\n");
                return leftType;
        }
        
    }
    printf("Error\n");
    exit(EXIT_FAILURE); 
}

/**
 * Verifica la compatibilidad de tipos en las sentencias 'return' de una función.
 *
 * Recorre recursivamente el AST (nodo 'root') buscando nodos de tipo NODE_RET.
 * Para cada nodo 'return' encontrado:
 *   - Obtiene el tipo de la expresión asociada mediante get_expression_type().
 *   - Verifica que ese tipo con y el tipo de retorno declarado de la función (f_returnType) coincidan.
 *
 * @param root Nodo raíz o subárbol actual del AST a analizar.
 * @param f_returnType Tipo de retorno declarado de la función en la que se encuentra.
 * @param stack Pila de tablas de símbolos, utilizada para verificar tipos de identificadores.
 */
void check_return_type(node* root, VarType f_returnType, tables_stack* stack) {
    if (root == NULL ) {
        return ;
    }

    if (root->type == NODE_RET) {
        VarType retType = get_expression_type(root, stack);
        if (retType !=  f_returnType) {
            printf("Error de tipo [línea %d, columna %d]: tipo de retorno incompatible\n", root->line, root->column);
            exit(EXIT_FAILURE);
        }
    } else {
        check_return_type(root->left, f_returnType, stack);
        check_return_type(root->right, f_returnType, stack);
    }
}