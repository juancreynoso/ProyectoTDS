%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sintax/sintax.tab.h"
FILE * lex_out;

int yycolumn = 1;  // columna inicial
void update_pos(const char* text, int length);
%}

%option noyywrap yylineno
%option nounput noinput

ID [a-zA-Z][a-zA-Z0-9_]*
digit [0-9]
val_bool true|false

%%

"Program"   { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "PROGRAM \n");return PROGRAM; }
"extern"    { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "EXTERN \n"); return EXTERN; }
"void"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "VOID \n"); return VOID; }
"bool"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "BOOL \n"); return BOOL; }
"integer"   { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "INT \n"); return INT; }
"return"    { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "RETURN \n"); return RETURN; }
"const"     { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "CONST \n"); return CONST; }
"if"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "IF \n"); return IF; }
"else"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "ELSE \n"); return ELSE; }
"then"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "THEN \n"); return THEN; }
"while"     { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "WHILE \n"); return WHILE; }

"=="        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "EQUALS \n"); return EQUALS; }
"="         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "ASSIGN \n"); return ASSIGN; }
"+"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "PLUS \n"); return PLUS; }
"-"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "SUB \n"); return SUB; }
"*"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "MULT \n"); return MULT; }
"/"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "DIV \n"); return DIV; }
"%"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "REST \n"); return REST; }
"<"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "LT \n"); return LT; }
">"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "GT \n"); return GT; }
"||"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "OR \n"); return OR; }
"&&"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "AND \n"); return AND; }
"!"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "NOT \n"); return NOT; }

"("         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "PAREN_L \n"); return PAREN_L; }
")"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "PAREN_R \n"); return PAREN_R; }
"{"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "LLAVE_L \n"); return LLAVE_L; }
"}"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "LLAVE_R \n"); return LLAVE_R; }
","         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, ", \n"); return ','; }
";"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); fprintf(lex_out, "PYC \n"); return PYC; }

{val_bool}  { 
                yylval.ival = (strcmp("true", yytext)==0)?1:0; 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn; 
                update_pos(yytext, yyleng);
                fprintf(lex_out, "VAL_BOOL \n");
                return VAL_BOOL; 
            }
{digit}+    { 
                yylval.ival = atoi(yytext); 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn;
                update_pos(yytext, yyleng);
                fprintf(lex_out, "NUM \n");
                return NUM; 
            }
{ID}        { 
                yylval.sval = strdup(yytext); 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn;
                update_pos(yytext, yyleng);
                fprintf(lex_out, "ID \n");
                return ID; 
            }

[ \t]+     { update_pos(yytext, yyleng); }

"//".*    { update_pos(yytext, yyleng); }
"/*"([^*]|\*+[^*/])*\*+"/" { update_pos(yytext, yyleng); }

\n         {yycolumn = 1; }

.          { printf("Error léxico: caracter no reconocido '%s' en linea %d, columna %d\n", yytext, yylineno, yycolumn); exit(EXIT_FAILURE); }

%%

// Función para actualizar línea y columna según el texto del token
void update_pos(const char* text, int length) {
    int col = yycolumn; // columna de inicio del token
    for (int i = 0; i < length; i++) {
        if (text[i] == '\n') {
            yylineno++;
            col = 1;  // reinicio columna
        } else {
            col++;
        }
    }
    yycolumn = col;
}


void set_file(FILE *out) {
    lex_out = out; 
}
