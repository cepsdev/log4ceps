#
# cepsvalidate - Makefile
#

includes :=  -I"include" -I"../../ceps/core/include" -I"../../ceps/core/include/include-gen"
cflags := -O0 -g3 -Wall -fmessage-length=0 -std=c++11
OBJDIR := bin
objfiles := $(patsubst %,$(OBJDIR)/%,$(objfiles))
CEPSLIB := ../../ceps/core/bin/libcepscore.a
tutorial_dir := tutorial
cepslibs := ../../ceps/core/bin

all: x86_linux64bit/logspec2cpp

x86_linux64bit/logspec2cpp: x86_linux64bit/main.o x86_linux64bit/ast_computations.o x86_linux64bit/cmdline_utils.o 
	g++   $(cflags) $(includes) $(cepslibs)/pugixml.o $(cepslibs)/ceps_ast.o $(cepslibs)/ceps.tab.o $(cepslibs)/ceps_interpreter.o $(cepslibs)/cepsparserdriver.o $(cepslibs)/cepsruntime.o $(cepslibs)/cepslexer.o $(cepslibs)/symtab.o $(cepslibs)/ceps_interpreter_loop.o $(cepslibs)/ceps_interpreter_nodeset.o x86_linux64bit/main.o x86_linux64bit/ast_computations.o x86_linux64bit/cmdline_utils.o  -o x86_linux64bit/logspec2cpp 

x86_linux64bit/main.o: src/main.cpp
	g++   $(cflags) $(includes) src/main.cpp -c -o x86_linux64bit/main.o

x86_linux64bit/ast_computations.o: src/ast_computations.cpp
	g++   $(cflags) $(includes) src/ast_computations.cpp -c -o x86_linux64bit/ast_computations.o

x86_linux64bit/cmdline_utils.o: src/cmdline_utils.cpp
	g++   $(cflags) $(includes) src/cmdline_utils.cpp -c -o x86_linux64bit/cmdline_utils.o

clean:
	rm -f *o
	rm -f def2cpp

