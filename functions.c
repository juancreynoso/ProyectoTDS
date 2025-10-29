#include <stdio.h>
#include "functions.h"

void print(int n) {
    printf("%d\n", n);
}

int get_int() {
    int x;
    scanf("%d", &x);

    return x;
}
