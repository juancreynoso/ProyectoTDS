#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "ast.h"
#include "symbol_table.h"

typedef struct tables_stack tables_stack;

VarType get_expression_type(node* root, tables_stack* stack);
void check_return_type(node* root, VarType f_returnType, tables_stack* stack);

#endif