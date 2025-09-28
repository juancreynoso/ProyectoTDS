// common.h
#ifndef COMMON_H
#define COMMON_H

typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    NONE
} VarType;

char* type_to_string(int type);

#endif