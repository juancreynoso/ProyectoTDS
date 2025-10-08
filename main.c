#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic_analyzer.h"
#include "tac_generator.h"

int yyparse(void);
void set_file(FILE *out);
extern node* root;
extern FILE* yyin;
extern int yylineno;
int yylex(void);

/**
 * Funcion que devuelve una cadena con indicaciones para el usuario segun un valor entero.
 * @param cod_msg Valor entero que indica el codigo del mensaje a retornar.
 * @return Cadena con indicaciones para el usuario.
 */
 
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
            " Etapas: [lex, parse, sem, tac, ass, opt] \n"
            "  c-tds -target <etapa> <archivo>\n\n";
            break;
        default:
            msg = "";
            printf("cod invalido \n");
            break;
    }
    return msg;
}

/**
 * Funcion que chequea el tipo de extension del archivo, asi tambien como su caracter inicial.
 * @param filename
 * @param ext Extension del archivo.
 * @return 1 si los chequeos son validos; 0 si los chequeos fallan.
 */

void check_filename(char *filename, char *ext){
    // Se chequea el simbolo inicial del archivo
    char* name = strrchr(filename, '/');

    if (name) {
        name++;
    } else {
        name = filename;
    }

    if (name[0] == '-') {
        printf("Error. El simbolo inicial del archivo es '-' es invalido \n");
        exit(EXIT_FAILURE);
        return;
    }

    const char *dot = strrchr(name, '.');
    if (!dot) {
        exit(EXIT_FAILURE);
        return;
    }

    // Se chequea la extension del archivo
    if (strcmp(dot + 1, ext) == 0) {
        return;
    }
    printf("Error. La extension del archivo debe ser '.ctds' \n");
    exit(EXIT_FAILURE);
    return;
}

/**
 * Funcion principal que ejecuta el compilador
 */

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

            check_filename(argv[3], "ctds");

            yyin = input_file;
            yylineno = 1;

            char *stage = argv[2];

            if (strcmp(stage, "lex") == 0) {
                printf(">> Ejecutando Analizador Lexico...\n");
                
                FILE *lex_out = fopen("outputs/output.lex", "w");
                
                set_file(lex_out);
                while (yylex() != 0) {
                    yylex();
                }
                fclose(lex_out);
                printf(">> Analisis correcto, sin errores lexicos\n");

            } else if (strcmp(stage, "parse") == 0) {
                printf(">> Ejecutando Analizador Lexico...\n");
                printf(">> Ejecutando Parser...\n");
                
                FILE *lex_out = fopen("outputs/output.lex", "w");
                FILE *parser_out = fopen("outputs/output.sint", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    printf("Parseado correctamente, sin errores sintacticos.\n");
                    save_ast(root, 0, parser_out);
                }   
                fclose(lex_out);
                fclose(parser_out); 
            } else if (strcmp(stage, "sem") == 0) {
                
                FILE *lex_out = fopen("outputs/output.lex", "w");
                FILE *parser_out = fopen("outputs/output.sint", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    printf("Parseado correctamente, sin errores sintactico.\n");
                    save_ast(root, 0, parser_out);
                    printf("Realizando analisis semantico...\n");
                    analyze_semantics(root);
                }  
                fclose(lex_out);
                fclose(parser_out); 
            } else if (strcmp(stage, "tac") == 0) {
        
                FILE *lex_out = fopen("outputs/output.lex", "w");
                FILE *parser_out = fopen("outputs/output.sint", "w");
                FILE *tac_out = fopen("outputs/output.ci", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    printf("Parseado correctamente, sin errores sintactico.\n");
                    save_ast(root, 0, parser_out);
                    printf("Realizando analisis semantico...\n");
                    analyze_semantics(root);
                    tac_code(root, tac_out);
                }  
                fclose(lex_out);
                fclose(parser_out);
                fclose(tac_out);
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

            check_filename(argv[2], "ctds");

            FILE *lex_out = fopen("outputs/output.lex", "w");
            FILE *parser_out = fopen("outputs/output.sint", "w"); 
            FILE *tac_out = fopen("outputs/output.ci", "w");   
            set_file(lex_out);

            yyin = input_file;
            yylineno = 1;

            if (yyparse() == 0) {
                printf("Parseado correctamente, sin errores sintactico.\n");
                save_ast(root, 0, parser_out);
                analyze_semantics(root);
                tac_code(root, tac_out);
            }
            fclose(lex_out);
            fclose(parser_out); 
            fclose(tac_out);
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