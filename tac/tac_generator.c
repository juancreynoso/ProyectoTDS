#include <stdio.h>
#include <stdlib.h>
#include "tac_generator.h"

/* Contadores de temporales y labels */
static int temp_count = 0;
static int label_count = 0;

/**
 * Genera un nuevo temporal.
 * @return Nuevo temporal.
 */
char* new_temp() {
    char* name = malloc(10);
    sprintf(name, "t%d", temp_count++);
    return name;
}

/**
 * Genera una nueva etiqueta.
 * @return Nueva etiqueta.
 */
char* new_label() {
    char* name = malloc(10);
    sprintf(name, "L%d", label_count++);
    return name;
}

/**
 * Genera el código intermedio en forma de lista de instrucciones TAC
 * a partir del AST y lo guarda en un archivo.
 * @param root Nodo raíz del AST del programa.
 * @param tac_out Archivo de salida donde se escribirá el código TAC.
 */
instruction_list* tac_code(node* root, FILE* tac_out) {
    instruction_list* list = init_instruction_list();

    generate_tac_from_ast(root, &list);
    save_instruction_list(list, tac_out);
    
    return list;
}

/**
 * Recorre los nodos del ast, va generando el codigo de tres direcciones y almacenandolo en la lista de instrucciones.
 * @param root Nodo actual del AST a procesar 
 * @param list Lista donde se almacenan las instruccioens.
 */
void generate_tac_from_ast(node* root, instruction_list **list) {
    if (root == NULL) {
        return;
    }

    switch(root->type){
        case NODE_DECL_METH: {

            operand op1;
            op1.info = root->info;
            op1.class = OPE_DECL_METH;
            op1.name = root->info->METH_DECL.name;

            instruction start_func;
            start_func.type = FUNC;
            start_func.op1 = op1;
            insert_instruction(list, start_func);

            if (root->info->METH_DECL.f_params != NULL) {
                Node_P_List* param_cursor = root->info->METH_DECL.f_params->head;
                while (param_cursor != NULL) {
                    operand param;
                    param.info = malloc(sizeof(union type));
                    param.class = OPE_PARAM;
                    param.name = param_cursor->p.name;
                    param.info->ID.name = param_cursor->p.name;
                    param.info->ID.type = param_cursor->p.type;
                    
                    instruction i_param;
                    i_param.type = PARAM;
                    i_param.op1 = param;
                    insert_instruction(list, i_param);
                    
                    param_cursor = param_cursor->next;
                }
            }

            generate_tac_from_ast(root->left, list); // Entro al bloque

            instruction end_func;
            end_func.type = FFUNC;           
            insert_instruction(list, end_func);

            break;
        }   

        case NODE_IF_ELSE: {

            instruction i_cond;
            i_cond.type = IF_COND;

            operand cond = generate_tac_from_expression(root->info->IF_ELSE.expr, list);
            operand lbl;
            lbl.name = new_label();
            lbl.class = OPE_LABEL;

            i_cond.op1 = cond;
            i_cond.op2 = lbl;

            insert_instruction(list, i_cond);

            // Verifico si es if then o if then else

            if (root->info->IF_ELSE.else_block == NULL) {

                // GOTO END
                instruction end_gt;
                operand end_lbl;
                end_lbl.class = OPE_LABEL;
                end_lbl.name = new_label();
                end_gt.type = GOTO;
                end_gt.op1 = end_lbl;

                insert_instruction(list, end_gt);
                
                // Bloque IF_THEN 
                instruction if_label;
                if_label.type  = LABEL;
                if_label.op1 = lbl;

                insert_instruction(list, if_label);

                generate_tac_from_ast(root->info->IF_ELSE.if_block, list);

                instruction end_label;
                end_label.type  = LABEL;
                end_label.op1 = end_lbl;

                insert_instruction(list, end_label);

            } else {
                // GOTO ELSE
                instruction else_gt;
                operand else_lbl;
                else_lbl.class = OPE_LABEL;
                else_lbl.name = new_label();
                else_gt.type = GOTO;
                else_gt.op1 = else_lbl;

                insert_instruction(list, else_gt);
                
                // Bloque IF_THEN 
                instruction if_label;
                if_label.type  = LABEL;
                if_label.op1 = lbl;

                insert_instruction(list, if_label);

                generate_tac_from_ast(root->info->IF_ELSE.if_block, list);

                // GOTO END
                instruction end_gt;
                operand end_lbl;
                end_lbl.class = OPE_LABEL;
                end_lbl.name = new_label();
                end_gt.type = GOTO;
                end_gt.op1 = end_lbl;

                insert_instruction(list, end_gt);

                // Bloque ELSE
                instruction else_label;
                else_label.type = LABEL;
                else_label.op1 = else_lbl;

                insert_instruction(list, else_label);

                generate_tac_from_ast(root->info->IF_ELSE.else_block, list);

                instruction end_label;
                end_label.type = LABEL;
                end_label.op1 = end_lbl;

                insert_instruction(list, end_label);
                
            }

            break;
        } 

        case NODE_WHILE: {
            operand start_label;
            start_label.class = OPE_LABEL;
            start_label.name = new_label();
            
            instruction i_start;
            i_start.type = LABEL; // label inicio while
            i_start.op1 = start_label;
            insert_instruction(list, i_start);
            
            operand cond = generate_tac_from_expression(root->info->WHILE.expr, list);
            
            operand end_label;
            end_label.class = OPE_LABEL;
            end_label.name = new_label();
            
            instruction i_cond;
            i_cond.type = IF_FALSE_GOTO; // ifFalse cond GOTO L1
            i_cond.op1 = cond;
            i_cond.op2 = end_label;
            insert_instruction(list, i_cond);
            
            generate_tac_from_ast(root->info->WHILE.block, list);
            
            instruction i_goto;
            i_goto.type = GOTO; // GOTO L0
            i_goto.op1 = start_label;
            insert_instruction(list, i_goto);
            
            instruction i_end;
            i_end.type = LABEL; // Label fin while
            i_end.op1 = end_label;
            insert_instruction(list, i_end);
        }

        case NODE_OP:
            switch(root->info->OP.name){
                case OP_ASSIGN: {
                    operand t1 = generate_tac_from_expression(root->right, list); 
                    operand id = generate_tac_from_expression(root->left, list);

                    instruction i;
                    i.type = op_name_to_inst_type(root->info->OP.name);
                    i.op1 = id; 
                    i.op2 = t1;

                    insert_instruction(list, i);

                    break;                    
                }
                default:
                    break;
            }
            break;
        
        case NODE_CALL_METH: {
            generate_tac_from_expression(root, list);
            break;
        }
            
        case NODE_RET: {
            operand t1 = generate_tac_from_expression(root->left, list);
            instruction i;
            i.type = RET;
            i.op1 = t1;
            insert_instruction(list, i);

            break;
        }

        default:
            generate_tac_from_ast(root->left, list);
            generate_tac_from_ast(root->right, list);
            break;
    }

}
 
/**
 * Funcion auxiliar que genera codigo intermedio de expresiones
 * @param root Nodo actual del AST a procesar.
 * @param list Lista donde se almacenan las instrucciones.
 * @return Operando.
 */
operand generate_tac_from_expression(node* root, instruction_list **list) {
    switch(root->type){
        case NODE_NUM: {
            operand op;
            op.class = OPE_NUM;
            op.info = root->info;
            return op;
        }

        case NODE_BOOL: {
            operand op;            
            op.class = OPE_BOOL;
            op.info = root->info;
            return op;
        } 

        case NODE_DECL:
        case NODE_ID_USE: {
            operand op;
            op.class = OPE_VAR;
            op.info = root->info;
            return op; 
        }
            
        case NODE_OP: {
            switch(root->info->OP.name) {
                case OP_PLUS: 
                case OP_SUB:
                case OP_MULT:
                case OP_DIV: 
                case OP_REST:
                case OP_GT:
                case OP_LT:
                case OP_EQUALS:
                case OP_AND:
                case OP_OR:{
                    operand left = generate_tac_from_expression(root->left, list);
                    operand right = generate_tac_from_expression(root->right, list);
                    operand t1;
                    t1.class = OPE_TEMP;
                    t1.name = new_temp();
                    t1.info = root->info;

                    instruction i;
                    i.type = op_name_to_inst_type(root->info->OP.name);
                    i.op1 = left; 
                    i.op2 = right;
                    i.result = t1;
                    insert_instruction(list, i);

                    return t1;
                }
                case OP_NOT:
                case OP_MINUS: {
                    operand left = generate_tac_from_expression(root->left, list);
                    operand t1;
                    t1.class = OPE_TEMP;
                    t1.name = new_temp();
                    t1.info = root->info;

                    instruction i;
                    i.type = op_name_to_inst_type(root->info->OP.name);
                    i.op1 = left; 
                    i.result = t1;
                    insert_instruction(list, i);

                    return t1;
                }
                default:
                    break;
            }
            break;
        }

        case NODE_CALL_METH: {
            if (root->info->METH_CALL.c_params != NULL) {
                Node_C_List* param_cursor = root->info->METH_CALL.c_params->head;
                while (param_cursor != NULL) {
                    operand param_value = generate_tac_from_expression(param_cursor->p, list);
                    
                    instruction i_param;
                    i_param.type = LOAD;
                    i_param.op1 = param_value;
                    insert_instruction(list, i_param);
                    
                    param_cursor = param_cursor->next;
                }
            }
            operand temp; // Temp para el resultado del metodo
            temp.class = OPE_TEMP;
            temp.name = new_temp();
            
            operand op_call;
            op_call.class = OPE_CALL_METH;
            op_call.name = root->info->METH_CALL.name;
            
            instruction i_call;
            i_call.type = CALL;
            i_call.op1 = op_call;
            i_call.result = temp;
            insert_instruction(list, i_call);
            
            return temp;
        }
        default:
            generate_tac_from_expression(root->left, list);
            generate_tac_from_expression(root->right, list);
            break;
    }
    operand def = {0};
    return def;
}

/**
 * Funcion que inserta una instruccion en una lista de instrucciones
 * @param list Lista de instrucciones.
 * @param i Nueva instruccion.
 */
void insert_instruction(instruction_list** list, instruction i) {
    instruction_node* new_it = malloc(sizeof(instruction_node));
    new_it->i = i;
    new_it->next = NULL;

    instruction_node* temp = (*list)->head;

    while (temp->next->i.type != END_PRG) {
        temp = temp->next;
    }

    new_it->next = temp->next;
    temp->next = new_it;

    (*list)->size++;
}

/**
 * Devuelve una lista de instrucciones inicializada.
 * @return Lista de instrucciones.
 */
instruction_list* init_instruction_list() {
    instruction_list* list = malloc(sizeof(instruction_list));
    list->head = NULL;
    list->size = 0;

    instruction i_begin;
    instruction i_end;
    i_begin.type = PRG;
    i_end.type = END_PRG;

    instruction_node* n_begin = malloc(sizeof(instruction_node));
    instruction_node* n_end = malloc(sizeof(instruction_node));

    n_begin->i = i_begin;
    n_end->i = i_end;
    n_begin->next = n_end;
    n_end->next = NULL;

    list->head = n_begin;
    list->size = 2;

    return list;
}

/**
 * Almacena la lista de instrucciones en un archivo.
 * @param list Lista de instrucciones que se almacenara.
 * @param tac_out Archivo en donde se almacenara la lista.
 */
void save_instruction_list(instruction_list* list,  FILE* tac_out) {
    if (list == NULL) {
        return;
    }

    instruction_node* cursor = list->head;
    while(cursor != NULL){
        instruction i = cursor->i;
        char* i_str = instruction_representation(i);
        fprintf(tac_out, "%s", i_str);
        free(i_str);
        cursor = cursor->next;
    }
}

/**
 * Dado un operando devuelve su representacion en formato de cadena.
 * @param op Operando.
 * @return Cadena que representa un operando.
 */
char* operand_to_str(operand op) {
    char* buffer = malloc(64);
    switch(op.class) {
        case OPE_NUM:
            snprintf(buffer, 64, "%d", op.info->INT.value);
            break;
        case OPE_BOOL:
            snprintf(buffer, 64, "%s", op.info->BOOL.value ? "true" : "false");
            break;
        case OPE_TEMP:
            snprintf(buffer, 64, "%s", op.name);
            break;
        case OPE_VAR:
            snprintf(buffer, 64, "%s", op.info->ID.name);
            break;
        case OPE_DECL_METH:
            snprintf(buffer, 64, "%s", op.name);
            break;
        case OPE_CALL_METH:
            snprintf(buffer, 64, "%s", op.name);
            break;
        case OPE_LABEL:
            snprintf(buffer, 64, "%s", op.name);
            break;
        case OPE_PARAM:
            snprintf(buffer, 64, "%s", op.name);
            break;
        default:
            snprintf(buffer, 64, "???");
            break;
    }
    return buffer;
}

/**
 * Dada una instrucción devuelve su representacion en formato de cadena.
 * @param i Instrucción que se representara como una cadena.
 * @return Devuelve una cadena que especifica una instrucción.
 */
char* instruction_representation(instruction i) {
    char* buffer = malloc(128);
    
    char* op1_str;
    char* op2_str;
    char* result_str;

    switch(i.type) {
        case PRG:
            snprintf(buffer, 128, "PROGRAM\n");
            break;
        case END_PRG:
            snprintf(buffer, 128, "END\n");
            break;     
        case RET: {
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "RET %s\n", op1_str);
            free(op1_str);
            break;
        }
        case PLUS:
        case SUB:
        case MULT:
        case DIV:
        case REST:
        case GT:
        case LT:
        case EQUALS:
        case AND:
        case OR:
            op1_str = operand_to_str(i.op1);
            op2_str = operand_to_str(i.op2);
            result_str = operand_to_str(i.result);
            snprintf(buffer, 128, "%s := %s %s %s\n",
                    result_str,
                    op1_str,
                    op_to_tr(i.type),
                    op2_str);
            free(op1_str);
            free(op2_str);
            free(result_str);
            break;
        case MINUS:
        case NOT:
            op1_str = operand_to_str(i.op1);
            result_str = operand_to_str(i.result);
            snprintf(buffer, 128, "%s := %s %s\n",
                    result_str,
                    op_to_tr(i.type),
                    op1_str);
            free(op1_str);
            free(result_str);
            break;
        case ASSIGN:
            op1_str = operand_to_str(i.op1);
            op2_str = operand_to_str(i.op2);
             snprintf(buffer, 128, "%s := %s\n",
                     op1_str,
                     op2_str);
            free(op1_str);
            free(op2_str);
            break;
        case FUNC:
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "FUNC %s \n", op1_str);
            free(op1_str);
            break;
        case FFUNC:
            snprintf(buffer, 128, "FFUNC\n");
            break;
        case IF_COND:
            op1_str = operand_to_str(i.op1);
            op2_str = operand_to_str(i.op2);

            snprintf(buffer, 128, "IF %s GOTO %s\n",
                 op1_str,
                 op2_str);

            break;
        case IF_FALSE_GOTO:
            op1_str = operand_to_str(i.op1); // condición
            op2_str = operand_to_str(i.op2); // etiqueta destino
            snprintf(buffer, 128, "ifFalse %s GOTO %s\n", op1_str, op2_str);
            free(op1_str);
            free(op2_str);
            break;
        case LABEL:
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "%s\n", op1_str);
            free(op1_str);
            break;
        case GOTO:
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "GOTO %s\n", op1_str);
            free(op1_str);
            break;
        case CALL: {
            op1_str = operand_to_str(i.op1);
            result_str = operand_to_str(i.result);
            snprintf(buffer, 128, "%s := CALL %s\n", result_str, op1_str);
            free(result_str);
            free(op1_str);
            break;
        }
        case PARAM:
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "PARAM %s\n", op1_str);
            break;
        case LOAD:
            op1_str = operand_to_str(i.op1);
            snprintf(buffer, 128, "LOAD %s\n", op1_str);
            break;
        default:
            snprintf(buffer, 128, "UNKNOWN\n");
            break;
    }

    return buffer;
}

/**
 * Devuelve una cadena que especifica un tipo de instrucción.
 * @param type Tipo de instrucción.
 * @return Cadena que especifica un tipo de instrucción.
 */
char* op_to_tr(instruction_type type) {
    switch(type){
        case PLUS:
            return "PLUS";
        case SUB:
            return "SUB";
        case MULT:
            return "MULT";
        case DIV:
            return "DIV";
        case REST:
            return "REST";
        case GT:
            return "GT";
        case LT:
            return "LT";
        case EQUALS:
            return "EQUALS";
        case AND:
            return "AND";
        case OR:
            return "OR";
        case MINUS:
            return "MINUS";
        case NOT:
            return "NOT";
        default:
            return "?";
    }
}

/**
 * Devuelve un tipo de instrucción.
 * @param type Valor que indica un tipo de operacion en el AST.
 * @return Valor que representa una instrucción en el enumerado instruction_type.
 */
instruction_type op_name_to_inst_type(OpType type) {
    switch(type) {
        case OP_ASSIGN:
            return ASSIGN;
        case OP_PLUS:
            return PLUS;
        case OP_SUB:
            return SUB;
        case OP_MULT:
            return MULT;
        case OP_DIV:
            return DIV;
        case OP_REST:
            return REST;
        case OP_MINUS:
            return MINUS;
        case OP_GT:
            return GT;
        case OP_LT:
            return LT;
        case OP_EQUALS:
            return EQUALS;
        case OP_AND:
            return AND;
        case OP_OR:
            return OR;
        case OP_NOT:
            return NOT;
    }
}
