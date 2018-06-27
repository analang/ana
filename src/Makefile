CC = cc
PROFILE=-pg
CFLAGS = -O2 -Iinclude -Wall -Wextra -g -ggdb -std=c99
YFLAGS=--defines=include/ana_parser.h --output=parser/ana_parser.c
LFLAGS=--noline --outfile=parser/ana_lexer.c --header-file=include/ana_lexer.h
LDLIBS = -lm
LDFLAGS=-rdynamic
OBJECTS=ana_parser.o ana_lexer.o ana.o ast.o arena.o compile.o \
array.o bool.o code.o container.o double.o frame.o function.o long.o map.o \
string.o builtins.o ana_debug.o class.o module.o vm.o anaruntime.o anaio.o

MEM_OBJECTS=anamem.o ana_parser.o ana_lexer.o ast.o arena.o compile.o \
array.o bool.o code.o container.o double.o frame.o function.o long.o map.o \
string.o builtins.o ana_debug.o class.o module.o vm.o anaruntime.o anaio.o

PREFIX=/usr/local

ana: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ../bin/$@ \
	../bin/ana_parser.o \
	../bin/ana_lexer.o \
	../bin/anaio.o \
	../bin/ast.o \
	../bin/vm.o \
	../bin/ana.o \
	../bin/arena.o \
	../bin/compile.o \
	../bin/builtins.o \
	../bin/ana_debug.o \
	../bin/array.o \
	../bin/bool.o \
	../bin/code.o \
	../bin/container.o \
	../bin/double.o \
	../bin/frame.o \
	../bin/function.o \
	../bin/long.o \
	../bin/map.o \
	../bin/string.o \
	../bin/class.o \
	../bin/module.o \
	../bin/anaruntime.o \
	$(LDLIBS)
	make install
anamem: $(MEM_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ../bin/$@ \
	../bin/ana_parser.o \
	../bin/ana_lexer.o \
	../bin/anaio.o \
	../bin/ast.o \
	../bin/vm.o \
	../bin/anamem.o \
	../bin/arena.o \
	../bin/compile.o \
	../bin/builtins.o \
	../bin/ana_debug.o \
	../bin/array.o \
	../bin/bool.o \
	../bin/code.o \
	../bin/container.o \
	../bin/double.o \
	../bin/frame.o \
	../bin/function.o \
	../bin/long.o \
	../bin/map.o \
	../bin/string.o \
	../bin/class.o \
	../bin/module.o \
	../bin/anaruntime.o \
	$(LDLIBS)
	make install_ana_mem

anamem.o: programs/anamem.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

anaruntime.o: ana/anaruntime.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

anaio.o: ana/anaio.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

ana_parser.o: parser/ana_parser.c ana_lexer.o
	$(CC) $(CFLAGS) -c parser/ana_parser.c -o ../bin/ana_parser.o

ana_lexer.o: parser/ana_lexer.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

parser/ana_parser.c: parser/grammar.y
	$(YACC) $(YFLAGS) $<

parser/ana_lexer.c: parser/lexer.l
	$(LEX) $(LFLAGS) $<

ana.o: ana/ana.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

vm.o: vm/vm.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

ast.o: parser/ast.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

arena.o: ana/arena.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

ana_debug.o: ana/ana_debug.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

compile.o: compiler/compile.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

#vm.o: vm/vm.c
#	$(CC) $(CFLAGS) -c $< -o ../bin/$@

builtins.o: vm/builtins.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

array.o: objects/array.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

module.o: objects/module.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

bool.o: objects/bool.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

code.o: objects/code.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

container.o: objects/container.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

double.o: objects/double.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

frame.o: objects/frame.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

function.o: objects/function.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

long.o: objects/long.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

map.o: objects/map.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

string.o: objects/string.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

class.o: objects/class.c
	$(CC) $(CFLAGS) -c $< -o ../bin/$@

install:
	mkdir -p $(HOME)/bin
	cp ../bin/ana $(HOME)/bin

install_ana_mem:
	mkdir -p $(HOME)/bin
	cp ../bin/anamem $(HOME)/bin

clean:
	rm -f ../bin/* parser/ana_parser.c include/ana_parser.h include/ana_lexer.h \
	parser/ana_lexer.c
