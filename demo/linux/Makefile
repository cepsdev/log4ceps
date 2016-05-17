#
# cepsvalidate - Makefile
#

includes :=   -I"generated_files" -I"../../include"
cflags := -O0 -g3 -Wall -fmessage-length=0 -std=c++11
OBJDIR := bin
objfiles := $(patsubst %,$(OBJDIR)/%,$(objfiles))
CEPSLIB := ../../ceps/core/bin/libcepscore.a
tutorial_dir := tutorial
cepslibs := ../../ceps/core/bin

all: demo reader

demo: main.o log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o 
	g++   $(cflags) $(includes) -lpthread  log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o  main.o -o demo

reader: generated_files/log4kmw_default_log_reader.cpp log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o log4kmw_default_log_reader.o
	g++   $(cflags) $(includes) -lpthread  log4kmw_default_log_reader.o log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o  -o reader

log4kmw_default_log_reader.o:
	g++   $(cflags) $(includes) generated_files/log4kmw_default_log_reader.cpp -c -o log4kmw_default_log_reader.o
main.o:
	g++   $(cflags) $(includes) main.cpp -c -o main.o

log4kmw_events.o:
	g++   $(cflags) $(includes) generated_files/log4kmw_events.cpp -c -o log4kmw_events.o

log4kmw_states.o:
	g++   $(cflags) $(includes) generated_files/log4kmw_states.cpp -c -o log4kmw_states.o

log4kmw_records.o:
	g++   $(cflags) $(includes) generated_files/log4kmw_records.cpp -c -o log4kmw_records.o

log4kmw_loggers.o:
	g++   $(cflags) $(includes) generated_files/log4kmw_loggers.cpp -c -o log4kmw_loggers.o


log4kmw_record.o:
	g++   $(cflags) $(includes) ../../src/log4kmw_record.cpp -c -o log4kmw_record.o

log4kmw_serialization.o:
	g++   $(cflags) $(includes) ../../src/log4kmw_serialization.cpp -c -o log4kmw_serialization.o

clean:
	rm -f *o
	rm -f demo

