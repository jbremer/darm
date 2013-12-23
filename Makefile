CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra -s

AUTOGEN = darm-internal.h darm-tables.c darm-tables.h darm-instr.h
OBJECTS = main.o darm.o darm-tables.o

default: $(AUTOGEN) main

main: $(OBJECTS)

$(AUTOGEN): generator.py tablegen.py tables
	python generator.py

clean:
	rm -f $(OBJECTS) $(AUTOGEN)
