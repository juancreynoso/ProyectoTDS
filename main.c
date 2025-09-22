#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic_analyzer.h"

int yyparse(void);
extern node* root;
extern FILE* yyin;
extern int yylineno;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s archivo\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    yyin = input_file;
    yylineno = 1;
    if (yyparse() == 0) {
        printf("Parseado correctamente, sin errores.\n");

        printf("\n--- AST ----\n");
        print_tree(root, 0);
        analyze_semantics(root);
    }

    fclose(input_file);
    return 0;
}
