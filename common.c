#include "common.h"

/** 
 * Funcion que devuelve un string por cada valor del enumerado VariableType.
 * @param type Entero que indica el tipo de dato. 
 * @return Cadena que indica un tipo de dato.
 */
 char* type_to_string(int type) {
    if (type == 0) return "integer";
    else if (type == 1) return "bool";
    return "void";
}

/**
 * Funcion que devuelve un string segun el tipo de operacion.
 * @param type Entero que indica el tipo de operacion.
 * @return Cadena que indica el tipo de operacion.
 */

char* op_name(int type) {
    switch (type) {
        case 0:
            return "=";
        case 1:
            return "+";
        case 2:
            return "-";
        case 3:
            return "*";
        case 4:
            return "/";
        case 5:
            return "%%";
        case 6:
            return "unary -";
        case 7:
            return ">";
        case 8:
            return "<";
        case 9:
            return "==";
        case 10:
            return "&&";
        case 11:
            return "||";
        case 12:
            return "!";
        default:
            return "unknown_op";
    }
}
