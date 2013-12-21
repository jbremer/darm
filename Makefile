CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra -s

AUTOGEN = darm-internal.h darm-tables.c darm-tables.h
OBJECTS = main.o darm.o darm-tables.o

default: $(AUTOGEN) main

main: $(OBJECTS)

darm-tables.c darm-tables.h: tblarmv7.py tablegen.py
	python tblarmv7.py

darm-internal.h:
	python internal.py

clean:
	rm -f $(OBJECTS) $(TABLES)
