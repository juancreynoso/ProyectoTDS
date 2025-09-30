#include "semantic_analyzer.h"

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
                            printf("Error de tipos. Operación: '%s' requiere tipo booleano\n", op_name(root->info->OP.name));
                            exit(EXIT_FAILURE);
                    }
                    return root->info->OP.type = leftType;
                    break;
                case OP_MINUS:
                    if (leftType != TYPE_INT) {
                            printf("Error de tipos. Operación: '%s' requiere tipo entero\n", op_name(root->info->OP.name));
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
                    printf("Error de tipos. Operación: '%s' requiere tipo entero\n", op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }

                root->info->OP.type = leftType;
                return leftType;
                break;         

            case OP_ASSIGN:
                if(leftType != rightType) {
                    printf("Error: tipos incompatibles en asignación, Esperado '%s', Recibido: '%s' \n", 
                type_to_string(leftType), type_to_string(rightType));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;
                break;   
            case OP_OR:
            case OP_AND:

                if(leftType != TYPE_BOOL || rightType!= TYPE_BOOL) {
                    printf("Error de tipos. Operación: '%s' requiere tipo booleano\n", op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = leftType;
                return leftType;
                break;
            case OP_EQUALS:
                root->info->OP.type = TYPE_BOOL;
                return TYPE_BOOL;
                break;
            case OP_GT:
            case OP_LT:
                root->info->OP.type = TYPE_BOOL;
                if(leftType != TYPE_INT || rightType != TYPE_INT) {
                    printf("Error de tipos. Operación: '%s' requiere tipo entero\n", op_name(root->info->OP.name));
                    exit(EXIT_FAILURE); 
                }
                root->info->OP.type = TYPE_BOOL;
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