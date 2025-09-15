%{
/* Definition section */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sintax/sintax.tab.h"

%}

%option noyywrap yylineno
%option nounput noinput

ID [a-zA-Z][a-zA-Z0-9_]*    
digit [0-9]
val_bool true|false

%%
"Program"   { printf("PROGRAM \n"); return PROGRAM; }
"extern"    { printf("EXTERN \n"); return EXTERN; }
"void"      { printf("VOID \n"); return VOID; }
"bool"      { printf("BOOL \n"); return BOOL; }
"integer"   { printf("INT \n"); return INT; }
"return"    { printf("RETURN \n"); return RETURN; }
"const"     { printf("CONST \n"); return CONST; }
"if"        { printf("IF \n"); return IF; }
"else"      { printf("ELSE \n"); return ELSE; }
"then"      { printf("THEN \n"); return THEN; }
"while"     { printf("WHILE \n"); return WHILE; }

"=="        { printf("EQUALS \n"); return EQUALS; }
"="         { printf("ASSIGN \n"); return ASSIGN; }
"+"         { printf("PLUS \n"); return PLUS; }
"-"         { printf("SUB \n"); return SUB; }
"*"         { printf("MULT \n"); return MULT; }
"/"         { printf("DIV \n"); return DIV; }
"%"         { printf("REST \n"); return REST; }
"<"         { printf("LT \n"); return LT; }
">"         { printf("GT \n"); return GT; }
"||"        { printf("OR \n"); return OR; }
"&&"        { printf("AND \n"); return AND; }
"!"         { printf("NOT \n"); return NOT; }

"("         { printf("PAREN_L \n"); return PAREN_L; }
")"         { printf("PAREN_R \n"); return PAREN_R; }
"{"         { printf("LLAVE_L \n"); return LLAVE_L; }
"}"         { printf("LLAVE_R \n"); return LLAVE_R; }
","         { printf("COMA \n"); return ','; }

{val_bool}  {  
                printf("VAL_BOOL \n"); 

                if(strcmp("true", yytext) == 0){
                    yylval.ival = 1;
                } else {
                    yylval.ival = 0;
                }
                return VAL_BOOL;
            }
{digit}+    {   
                printf("NUM \n"); 
                yylval.ival = atoi(yytext);
                return NUM;
            }
{ID}        { 
                printf("ID \n"); 
                yylval.sval = strdup(yytext);
                return ID;
            }

[ \t\n]+    { /* ignorar espacios, tabs y saltos de línea */ }

";"         { printf("PYC\n"); return PYC; }

.           { printf("Error léxico: caracter no reconocido '%s'\n", yytext);
              exit(EXIT_FAILURE); }

"//".*      { /* ignorar comentario de línea */ }
"/*"([^*]|\*+[^*/])*\*+"/"    { /* ignorar comentario multilínea */ }
%%