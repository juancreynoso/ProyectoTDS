#include <stdio.h>
#include <stdlib.h>
#include "tac_generator.h"


void gen_intermediate_code(node* root, instruction_list *list){
    if (root == NULL) {
        return;
    }

    switch(root->type){
        case NODE_OP:
            switch(root->info->OP.name) {
                case OP_ASSIGN:
                    break;
                case OP_PLUS:
                    printf("");
                    break;
                default:
                    break;
            }
            break;
        default:
            gen_intermediate_code(root->left, list);
            gen_intermediate_code(root->left, list);
            break;
    }


 }