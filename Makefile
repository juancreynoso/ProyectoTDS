# Compilador
CC = gcc
CFLAGS = -Wall -g -I. -Iast -Ist -Isintax -Ilexer -Isemantic -Icompile

# Archivos y carpetas
BISON = bison -v -d
FLEX = flex

SRC_DIRS = sintax lexer ast semantic compile
OBJS = sintax/sintax.tab.o lexer/lex.yy.o ast/ast.o main.o semantic/semantic_analyzer.o common.o param_list.o
TARGET = c-tds

# Regla principal
all: $(TARGET)

# Ejecutable final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Documentacion
open-doc: doc/index.html
	open doc/index.html

# Generar parser con bison
sintax/sintax.tab.c sintax/sintax.tab.h: sintax/sintax.y
	$(BISON) -o sintax/sintax.tab.c $<

# Generar lexer con Flex
lexer/lex.yy.c: lexer/lexer.l
	$(FLEX) -o $@ $<

# Compilar cada objeto
sintax/sintax.tab.o: sintax/sintax.tab.c
	${CC} ${CFLAGS} -c -o $@ $<

lexer/lex.yy.o: lexer/lex.yy.c
	${CC} ${CFLAGS} -c -o $@ $<

param_list.o: ast/param_list.c ast/param_list.h
	${CC} ${CFLAGS} -c -o $@ $<

ast/ast.o: ast/ast.c ast/ast.h
	${CC} ${CFLAGS} -c -o $@ $<

semantic/semantic_analyzer.o: semantic/semantic_analyzer.c semantic/semantic_analyzer.h
	${CC} ${CFLAGS} -c -o $@ $<

common.o: common.c
	${CC} ${CFLAGS} -c -o $@ $<

main.o: main.c
	${CC} ${CFLAGS} -c -o $@ $<

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET) sintax/sintax.tab.* lexer/lex.yy.c sintax/sintax.output