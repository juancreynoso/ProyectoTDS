# Compilador
CC = gcc
CFLAGS = -Wall -g -I. -Iast -Ist -Isintax -Ilexer -Isemantic -Icompile

# Archivos y carpetas
BISON = bison -v -d
FLEX = flex

SRC_DIRS = sintax lexer ast st semantic compile
OBJS = lexer/lex.yy.o

TARGET = parser

# Regla principal
all: $(TARGET)

# Ejecutable final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Documentacion
open-doc: doc/index.html
	open doc/index.html

# Generar lexer con Flex
lexer/lex.yy.c: lexer/lexer.l
	$(FLEX) -o $@ $<

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET) sintax/calc-sintaxis.tab.* lexer/lex.yy.c sintax/calc-sintaxis.output compile/assembly.txt
