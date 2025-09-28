#include "common.h"
/** 
 * Devuelve un string por cada valor del enumerado VariableType 
 * Utilizado para mostrar resultados mas claros 
 */
 char* type_to_string(int type) {
    if (type == 0) return "integer";
    else if (type == 1) return "bool";
    return "void";
}
