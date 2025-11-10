#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

void print(int n) {
    printf("%d\n", n);
}

int get_int() {
    int x;
    scanf("%d", &x);

    return x;
}

void assert(int result, int expected) {
    const char* GREEN = "\033[1;32m";
    const char* RED   = "\033[1;31m";
    const char* RESET = "\033[0m";

    if (result != expected) {
        printf("%s[FAIL]%s Resultado esperado: %d, obtenido: %d\n",
               RED, RESET, expected, result);
        exit(1);
    } else {
        printf("%s[OK]%s Resultado esperado: %d, obtenido: %d\n",
               GREEN, RESET, result, expected);
    }
}

