#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic_analyzer.h"
#include "tac_generator.h"
#include "set_offsets.h"
#include "assembler_generator.h"
#include "opt/optimizations.h"

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
            "  c-tds <archivo>\n"
            "  c-tds -target <etapa> <archivo>\n"
            "  c-tds -o <salida> <archivo>\n"
            "  c-tds -opt <archivo>\n"
            "  c-tds -debug <archivo>\n";
            break;
        case 1:
            msg = "Por favor utilice una de las siguientes opciones:\n"
            " Etapas: [lex, parse, sem, tac, ass] \n"
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

char* gen_output_file_name(char* filename) {

    char* name = strrchr(filename, '/');
    char result[128] = "";
    char* ret = malloc(200);
    if (name) {
        name++;
    } else {
        name = filename;
    }

    strcat(result, "output/");
    name = strtok(name, ".");
    strcat(name, ".s");
    strcat(result, name);
    snprintf(ret, 128, "%s", result);
    return ret;
}


/**
 * Funcion principal que ejecuta el compilador
 */

int main(int argc, char *argv[]) {
    
    if (argc == 1) {
        fprintf(stderr, "%s", invalidCommandMessage(0));
        exit(EXIT_FAILURE);
    }

    char* output_file_name;

    if (strcmp(argv[1], "-o") == 0) {
        printf("Renombre de archivo ejecutable a... \n");
        return 1;
    } else if (strcmp(argv[1], "-opt") == 0) {
        if (argc == 3) {
            printf(">> Ejecutando optimizaciones... \n");
            // No especificando etapa
            FILE *input_file = fopen(argv[2], "r");
            if (!input_file) {
                fprintf(stderr, "%s", invalidCommandMessage(0));
                exit(EXIT_FAILURE);
                return 1;
            }      

            check_filename(argv[2], "ctds");
            output_file_name = gen_output_file_name(argv[2]);

            FILE *lex_out = fopen("output/output.lex", "w");
            FILE *parser_out = fopen("output/output.sint", "w"); 
            FILE *semantic_out = fopen("output/output.sem", "w");
            FILE *tac_out = fopen("output/output.ci", "w");;
            FILE *ass_out = fopen(output_file_name, "w");

            set_file(lex_out);

            yyin = input_file;
            yylineno = 1;

            if (yyparse() == 0) {
                run_optimization(root);
                save_ast(root, 0, parser_out);
                set_offsets(root, 1);
                analyze_semantics(root, semantic_out);
                instruction_list* list = tac_code(root, tac_out);
                ass_gen(list, ass_out);
            }
            fclose(lex_out);
            fclose(parser_out); 
            fclose(semantic_out);
            fclose(tac_out);
            fclose(ass_out);
            fclose(input_file);  
        } else {
            fprintf(stderr, "%s", invalidCommandMessage(0));
            exit(EXIT_FAILURE);
            return 1;
        }
        
    } else if (strcmp(argv[1], "-debug") == 0) {
        printf(">> Generando archivos de debugging... \n");
        // No especificando etapa
        FILE *input_file = fopen(argv[2], "r");
        if (!input_file) {
            fprintf(stderr, "%s", invalidCommandMessage(0));
            exit(EXIT_FAILURE);
            return 1;
        }      

        check_filename(argv[2], "ctds");
        output_file_name = gen_output_file_name(argv[2]);

        FILE *lex_out = fopen("output/output.lex", "w");
        FILE *parser_out = fopen("output/output.sint", "w"); 
        FILE *semantic_out = fopen("output/output.sem", "w");
        FILE *tac_out = fopen("output/output.ci", "w");
        FILE *ass_out = fopen(output_file_name, "w");
        
        set_file(lex_out);

        yyin = input_file;
        yylineno = 1;

        if (yyparse() == 0) {
            save_ast(root, 0, parser_out);
            set_offsets(root, 0);
            analyze_semantics(root, semantic_out);
            instruction_list* list = tac_code(root, tac_out);
            ass_gen(list, ass_out);
        }
        fclose(lex_out);
        fclose(parser_out); 
        fclose(semantic_out);
        fclose(tac_out);
        fclose(ass_out);
        fclose(input_file);  

    } else if (strcmp(argv[1], "-target") == 0) {
        
        if (argc == 4) {
            // Especificando etapa
            FILE *input_file = fopen(argv[3], "r");
            if (!input_file) {
                perror("No se pudo abrir el archivo");
                return 1;
            }

            check_filename(argv[3], "ctds");
            output_file_name = gen_output_file_name(argv[3]);
            printf("Archivo de salida: %s\n",output_file_name);

            yyin = input_file;
            yylineno = 1;

            char *stage = argv[2];

            if (strcmp(stage, "lex") == 0) {
                printf(">> Ejecutando analizador léxico...\n");
                
                FILE *lex_out = fopen("output/output.lex", "w");
                
                set_file(lex_out);
                while (yylex() != 0) {
                    yylex();
                }
                fclose(lex_out);

            } else if (strcmp(stage, "parse") == 0) {
                printf(">> Ejecutando analizador sintáctico...\n");
                
                FILE *lex_out = fopen("output/output.lex", "w");
                FILE *parser_out = fopen("output/output.sint", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    save_ast(root, 0, parser_out);
                }   
                fclose(lex_out);
                fclose(parser_out); 

                remove("output/output.lex");

            } else if (strcmp(stage, "sem") == 0) {
                printf(">> Ejecutando analizador semántico...\n");
                FILE *lex_out = fopen("output/output.lex", "w");
                FILE *parser_out = fopen("output/output.sint", "w");
                FILE *semantic_out = fopen("output/output.sem", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    save_ast(root, 0, parser_out);
                    analyze_semantics(root, semantic_out);
                }  
                fclose(lex_out);
                fclose(parser_out); 
                fclose(semantic_out);

                remove("output/output.lex");
                remove("output/output.sint");
                
            } else if (strcmp(stage, "tac") == 0) {
                printf(">> Ejecutando generador de código intermedio...\n");
                FILE *lex_out = fopen("output/output.lex", "w");
                FILE *parser_out = fopen("output/output.sint", "w");
                FILE *semantic_out = fopen("output/output.sem", "w");
                FILE *tac_out = fopen("output/output.ci", "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    save_ast(root, 0, parser_out);
                    analyze_semantics(root, semantic_out);
                    tac_code(root, tac_out);
                }  
                fclose(lex_out);
                fclose(parser_out);
                fclose(semantic_out);
                fclose(tac_out);

                remove("output/output.lex");
                remove("output/output.sint");
                remove("output/output.sem");

            } else if (strcmp(stage, "ass") == 0) {
                printf(">> Ejecutando generador de código assembly...\n");
                FILE *lex_out = fopen("output/output.lex", "w");
                FILE *parser_out = fopen("output/output.sint", "w");
                FILE *semantic_out = fopen("output/output.sem", "w");
                FILE *tac_out = fopen("output/output.ci", "w");
                FILE *ass_out = fopen(output_file_name, "w");

                set_file(lex_out);
                
                if (yyparse() == 0) {
                    save_ast(root, 0, parser_out);
                    set_offsets(root, 0);
                    analyze_semantics(root, semantic_out);
                    instruction_list* list = tac_code(root, tac_out);
                    ass_gen(list, ass_out);
                }  
                fclose(lex_out);
                fclose(parser_out);
                fclose(semantic_out);
                fclose(tac_out);
                fclose(ass_out);

                remove("output/output.lex");
                remove("output/output.sint");
                remove("output/output.sem");
                remove("output/output.ci");

            } else{
                    fprintf(stderr, "%s", invalidCommandMessage(1));
                    exit(EXIT_FAILURE);
            } 
               
            fclose(input_file);  
        } else {
            fprintf(stderr, "%s", invalidCommandMessage(0));
            exit(EXIT_FAILURE); 
        }
    } else {
        // No especificando etapa
        FILE *input_file = fopen(argv[1], "r");
        if (!input_file) {
            fprintf(stderr, "%s", invalidCommandMessage(0));
            exit(EXIT_FAILURE);
            return 1;
        }      

        check_filename(argv[1], "ctds");
        output_file_name = gen_output_file_name(argv[1]);

        FILE *lex_out = fopen("output/output.lex", "w");
        FILE *parser_out = fopen("output/output.sint", "w"); 
        FILE *semantic_out = fopen("output/output.sem", "w");
        FILE *tac_out = fopen("output/output.ci", "w");
        FILE *ass_out = fopen(output_file_name, "w");
        
        set_file(lex_out);

        yyin = input_file;
        yylineno = 1;

        if (yyparse() == 0) {
            save_ast(root, 0, parser_out);
            set_offsets(root, 0);
            analyze_semantics(root, semantic_out);
            instruction_list* list = tac_code(root, tac_out);
            ass_gen(list, ass_out);
        }
        fclose(lex_out);
        fclose(parser_out); 
        fclose(semantic_out);
        fclose(tac_out);
        fclose(ass_out);
        fclose(input_file);  

        remove("output/output.lex");
        remove("output/output.sint");
        remove("output/output.sem");
        remove("output/output.ci");
    }

    return 0;
}