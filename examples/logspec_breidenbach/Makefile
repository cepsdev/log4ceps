#
# cepsvalidate - Makefile
#

includes :=   -I"generated_files" -I"/work/SVN/Log4KMW/log4kmw/include/"
cflags := -O0 -g3 -Wall -fmessage-length=0 -std=c++11 -DEMBEDDED_PPC -D__linux
OBJDIR := bin
objfiles := $(patsubst %,$(OBJDIR)/%,$(objfiles))
CEPSLIB := /work/SVN/Log4KMW/ceps/core/bin/libcepscore.a
tutorial_dir := tutorial
cepslibs := /work/SVN/Log4KMW/ceps/core/bin

all: demo reader

demo: main.o log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o 
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) -lpthread -pthread log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o  main.o -o demo

reader: generated_files/log4kmw_default_log_reader.cpp log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o log4kmw_default_log_reader.o
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) -lpthread -pthread log4kmw_default_log_reader.o log4kmw_events.o log4kmw_states.o log4kmw_records.o log4kmw_loggers.o log4kmw_record.o log4kmw_serialization.o  -o reader

log4kmw_default_log_reader.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) generated_files/log4kmw_default_log_reader.cpp -c -o log4kmw_default_log_reader.o
main.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) main.cpp -c -o main.o

log4kmw_events.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) generated_files/log4kmw_events.cpp -c -o log4kmw_events.o

log4kmw_states.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) generated_files/log4kmw_states.cpp -c -o log4kmw_states.o

log4kmw_records.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) generated_files/log4kmw_records.cpp -c -o log4kmw_records.o

log4kmw_loggers.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) generated_files/log4kmw_loggers.cpp -c -o log4kmw_loggers.o


log4kmw_record.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) /work/SVN/Log4KMW/log4kmw/src/log4kmw_record.cpp -c -o log4kmw_record.o

log4kmw_serialization.o:
	/work/toolchains/ppc/bin/powerpc-e300c3-linux-gnu-g++   $(cflags) $(includes) /work/SVN/Log4KMW/log4kmw/src/log4kmw_serialization.cpp -c -o log4kmw_serialization.o

clean:
	rm -f *o
	rm -f demo

