#include <stdio.h>
#include "set_offsets.h"

static int var_offset = 0;
static int param_offset = 8;

void reset_offsets(){
    var_offset = 0;
    param_offset = 8;    
}

int new_var_offset() {
    var_offset -= 8;
    return var_offset;
}

int new_param_offset() {
    param_offset += 8;
    return param_offset;
}

void set_offsets(node* root) {
//    
}