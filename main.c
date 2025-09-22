#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic_analyzer.h"

int yyparse(void);
extern node* root;
extern FILE* yyin;
extern int yylineno;
int yylex(void);

char* invalidCommandMessage(int cod_msg){
    char *msg = NULL;
    switch(cod_msg){
        case 0:
            msg = "Por favor utilice una de las siguientes opciones:\n"
            "  c-tds -target <etapa> <archivo>\n"
            "  c-tds -o <salida>\n"
            "  c-tds -opt [optimizacion]\n"
            "  c-tds -debug\n";
            break;
        case 1:
            msg = "Por favor utilice una de las siguientes opciones:\n"
            " Etapas: [lexer, parser, sem, ci, ass, opt] \n"
            "  c-tds -target <etapa> <archivo>\n\n";
            break;
        default:
            msg = "";
            printf("cod invalido \n");
            break;
    }
    return msg;
}

int main(int argc, char *argv[]) {
    
    if (argc == 1) {
        fprintf(stderr, "%s", invalidCommandMessage(0));
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-o") == 0) {
        printf("Renombre de archivo ejecutable a... \n");
        return 1;
    } else if (strcmp(argv[1], "-opt") == 0) {
        printf("Realizar optimizaciones... \n");
        return 1;
    } else if (strcmp(argv[1], "-debug") == 0) {
        printf("Imprimir informacion sobre debugging... \n");
        return 1;    
    } else if (strcmp(argv[1], "-target") == 0) {
        
        if (argc == 4) {
            // Especificando etapa
            FILE *input_file = fopen(argv[3], "r");
            if (!input_file) {
                perror("No se pudo abrir el archivo");
                return 1;
            }

            yyin = input_file;
            yylineno = 1;

            char *stage = argv[2];

            if (strcmp(stage, "lexer") == 0) {
                printf(">> Ejecutando Analizador Sintactico...\n");

                while (yylex() != 0) {
                    yylex();
                }
                printf(">> Analisis correcto, sin errores lexicos\n");

            } else if (strcmp(stage, "parser") == 0) {
                printf(">> Ejecutando Parser...\n");
                if (yyparse() == 0) {
                    printf("Parseado correctamente, sin errores sintactico.\n");
                    printf("\n--- AST ----\n");
                    print_tree(root, 0);
                }     
            }else{
                    fprintf(stderr, "%s", invalidCommandMessage(1));
                    exit(EXIT_FAILURE);
            } 
               
            fclose(input_file);
        } else if (argc == 3){
            // No especificando etapa
            FILE *input_file = fopen(argv[2], "r");
            if (!input_file) {
                perror("No se pudo abrir el archivo");
                return 1;
            }      

            yyin = input_file;
            yylineno = 1;

            if (yyparse() == 0) {
                printf("Parseado correctamente, sin errores sintactico.\n");
                printf("\n--- AST ----\n");
                print_tree(root, 0);
            }
            fclose(input_file);    
        } else {
            fprintf(stderr, "%s", invalidCommandMessage(0));
            exit(EXIT_FAILURE); 
        }
    } else {
        fprintf(stderr, "%s", invalidCommandMessage(0));
        exit(EXIT_FAILURE);
    }

    return 0;
}