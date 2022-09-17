OBJECTS = server.o functions.o functions.h

SOURCES = $(wildcard *.c)
EXECS = $(OBJECTS:%.c=%)

all: $(EXECS)
	cc $(OBJECTS) -o serve