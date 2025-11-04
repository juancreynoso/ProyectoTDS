#include <stdio.h>
#include "set_offsets.h"

static int var_offset = 0;
static int param_offset = 8;
static int max_offset = 0;
static offset_list *free_offsets = NULL;

/**
 * Resetea los offsets
 */
void reset_offsets(){
    var_offset = 0;
    param_offset = 8;    
}

/**
 * Genera un nuevo offset para una variable o temporal
 */
int new_var_offset() {
    print_offsets(free_offsets);
    if (!is_empty(free_offsets)){
        return get_offset(&free_offsets);
    } else {
        var_offset -= 8;
    }
    
    if (var_offset < max_offset) {
        max_offset = var_offset; 
    }
    return var_offset;
}


/**
 * Genera un nuevo offset para un parametro
 */
int new_param_offset() {
    param_offset += 8;
    return param_offset;
}

int get_frame_size() {
    return -max_offset;
}

/**
 * Se encarga de asignar offsets tanto a variables, parametros y temporales, para la generacion de assembler.
 * @param root Nodo raiz del AST.
 */
void set_offsets(node* root) {
    if (root == NULL) {
        return;
    }

    switch(root->type) {
        case NODE_DECL:
            root->info->ID.offset = new_var_offset(); 
            break;
        case NODE_CALL_METH:
            root->info->METH_CALL.offset = new_var_offset(); 
            if (root->info->METH_CALL.c_params != NULL) {
                Node_C_List* cursor = root->info->METH_CALL.c_params->head;
                while (cursor != NULL) {
                    set_offsets_actual_params(cursor->p);
                    cursor = cursor->next;
                }
            }
            break;
        case NODE_OP: {
            switch (root->info->OP.name) {
                case OP_PLUS:
                case OP_SUB:
                case OP_MULT:
                case OP_DIV:
                case OP_REST:
                case OP_MINUS:
                case OP_GT:
                case OP_LT:
                case OP_EQUALS:
                case OP_AND:
                case OP_OR:
                case OP_NOT: {
                    set_offsets(root->left);
                    set_offsets(root->right);

                    root->info->OP.offset = new_var_offset();
                    printf("Asigno offset a temporal (OP): %d\n", root->info->OP.offset);

                    if (root->left && root->left->type == NODE_OP) {
                        add_offset(&free_offsets, root->left->info->OP.offset);
                        printf("Libero offset %d (temp izquierdo)\n", root->left->info->OP.offset);
                    }
                    if (root->right && root->right->type == NODE_OP) {
                        add_offset(&free_offsets, root->right->info->OP.offset);
                        printf("Libero offset %d (temp derecho)\n", root->right->info->OP.offset);
                    }

                    break;
                }
                case OP_ASSIGN: {
                    set_offsets(root->right);
                    set_offsets(root->left);

                    // Si se asigna un temporal se libera el offset
                    if (root->right && root->right->type == NODE_OP) {
                        add_offset(&free_offsets, root->right->info->OP.offset);
                        printf("Libero offset %d por asignación\n", root->right->info->OP.offset);
                    }
                    break;
                }
                default:
                    set_offsets(root->left);
                    set_offsets(root->right);
                    break;
            }
            break;
        }


        case NODE_IF_ELSE:
            set_offsets(root->info->IF_ELSE.expr);
            set_offsets(root->info->IF_ELSE.if_block);
            set_offsets(root->info->IF_ELSE.else_block);
            break;
        case NODE_WHILE:
            set_offsets(root->info->WHILE.block);
            set_offsets(root->info->WHILE.expr);
            break;
        case NODE_DECL_METH:
            reset_offsets();
            int count = 0;
            if (root->info->METH_DECL.f_params == NULL) {
                set_offsets(root->left);
                root->info->METH_DECL.frame_size = get_frame_size();
                break;
            }

            Node_P_List* cursor =  root->info->METH_DECL.f_params->head;
            
            while (cursor != NULL) {
                if (count < 6) {
                    cursor->p.offset = new_var_offset();
                    count++;
                } else {
                    cursor->p.offset = new_param_offset();
                }
                cursor = cursor->next;
            }

            set_offsets(root->left);


            root->info->METH_DECL.frame_size = get_frame_size();
            break;
        default:
            set_offsets(root->left);
            set_offsets(root->right);
            break;
    }
}

/**
 * Se encarga de asignar offsets a los sub arboles que representan parametros actuales.
 * @param root Nodo raiz del subarbol que representa una expresion.
 */
void set_offsets_actual_params(node* root) {
    if (root == NULL) {
        return ;
    }    

    switch(root->type){
        case NODE_OP:
            root->info->OP.offset = new_var_offset();
            break;
        case NODE_CALL_METH:
            set_offsets(root);
            break;
        default:
            set_offsets_actual_params(root->left);
            set_offsets_actual_params(root->right);
            break;
    }
}


// Inicializar lista vacia
offset_list* init_offset_list() {
    return NULL;
}


int is_empty(offset_list *list) {
    return (list == NULL);
}

// Agrego un offset al inicio
void add_offset(offset_list **list, int offset) {
    offset_list *new_node = malloc(sizeof(offset_list));
    new_node->info = offset;
    new_node->next = *list;  // el nuevo apunta al anterior
    *list = new_node;        // actualiza cabeza
}

int get_offset(offset_list **list) {
    if (is_empty(*list)) {
        fprintf(stderr, "Error: no hay offsets disponibles.\n");
        exit(EXIT_FAILURE);
    }
    offset_list *temp = *list;
    int res = temp->info;
    *list = temp->next;
    free(temp);
    return res;
}

void print_offsets(offset_list *list) {
    printf("Offsets libres: ");
    while (list) {
        printf("%d ", list->info);
        list = list->next;
    }
    printf("\n");
}
