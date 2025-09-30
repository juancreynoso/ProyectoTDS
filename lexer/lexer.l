%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sintax/sintax.tab.h"

int yycolumn = 1;  // columna inicial
void update_pos(const char* text, int length);
%}

%option noyywrap yylineno
%option nounput noinput

ID [a-zA-Z][a-zA-Z0-9_]*
digit [0-9]
val_bool true|false

%%

"Program"   { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return PROGRAM; }
"extern"    { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return EXTERN; }
"void"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return VOID; }
"bool"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return BOOL; }
"integer"   { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return INT; }
"return"    { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return RETURN; }
"const"     { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return CONST; }
"if"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return IF; }
"else"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return ELSE; }
"then"      { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return THEN; }
"while"     { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return WHILE; }

"=="        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return EQUALS; }
"="         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return ASSIGN; }
"+"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return PLUS; }
"-"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return SUB; }
"*"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return MULT; }
"/"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return DIV; }
"%"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return REST; }
"<"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return LT; }
">"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return GT; }
"||"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return OR; }
"&&"        { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return AND; }
"!"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return NOT; }

"("         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return PAREN_L; }
")"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return PAREN_R; }
"{"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return LLAVE_L; }
"}"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return LLAVE_R; }
","         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return ','; }
";"         { yylloc.first_line=yylineno; yylloc.first_column=yycolumn; update_pos(yytext, yyleng); return PYC; }

{val_bool}  { 
                yylval.ival = (strcmp("true", yytext)==0)?1:0; 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn; 
                update_pos(yytext, yyleng);
                return VAL_BOOL; 
            }
{digit}+    { 
                yylval.ival = atoi(yytext); 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn;
                update_pos(yytext, yyleng);
                return NUM; 
            }
{ID}        { 
                yylval.sval = strdup(yytext); 
                yylloc.first_line=yylineno; yylloc.first_column=yycolumn;
                update_pos(yytext, yyleng);
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

