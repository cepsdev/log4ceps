#
# cepsvalidate - Makefile
#

includes :=  -I"../../include" -I"../../../ceps/core/include" -I"../../../ceps/core/include/include-gen"
cflags := -O0 -g3 -Wall -fmessage-length=0 -std=c++11
OBJDIR := bin
objfiles := $(patsubst %,$(OBJDIR)/%,$(objfiles))
CEPSLIB := ../../../ceps/core/bin/libcepscore.a
tutorial_dir := tutorial
cepslibs := ../../../ceps/core/bin

all: dynbitset_tests

dynbitset_tests: dynamic_bitset_tests.o log4ceps_dynamic_bitset.o log4ceps_serialization.o
	g++   $(cflags) $(includes) $(cepslibs)/pugixml.o $(cepslibs)/ceps_ast.o $(cepslibs)/ceps.tab.o $(cepslibs)/ceps_interpreter.o $(cepslibs)/cepsparserdriver.o $(cepslibs)/cepsruntime.o $(cepslibs)/cepslexer.o $(cepslibs)/symtab.o $(cepslibs)/ceps_interpreter_loop.o $(cepslibs)/ceps_interpreter_nodeset.o $(cepslibs)/ceps_interpreter_macros.o $(cepslibs)/ceps_interpreter_functions.o dynamic_bitset_tests.o log4ceps_dynamic_bitset.o log4ceps_serialization.o -o dynbitset_tests 


dynamic_bitset_tests.o: dynamic_bitset_tests.cpp ../../include/log4ceps_dynamic_bitset.hpp
	g++   $(cflags) $(includes) dynamic_bitset_tests.cpp -c -o dynamic_bitset_tests.o

log4ceps_dynamic_bitset.o: ../../src/log4ceps_dynamic_bitset.cpp
	g++ $(cflags) $(includes) ../../src/log4ceps_dynamic_bitset.cpp -c -o log4ceps_dynamic_bitset.o

log4ceps_serialization.o: ../../src/log4ceps_serialization.cpp
	g++ $(cflags) $(includes) ../../src/log4ceps_serialization.cpp -c -o log4ceps_serialization.o 
clean:
	rm -f *o
	rm -f dynamic_bitset_tests

