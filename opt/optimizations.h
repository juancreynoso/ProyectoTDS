#ifndef OPTIMIZATIONS_H
#define OPTIMIZATIONS_H

#include "ast.h"


node* optimize_constants(node* root);
node* remove_dead_code(node* root);
void run_optimization(node* root);
void delete_subtree(node* root);

#endif