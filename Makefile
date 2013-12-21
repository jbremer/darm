CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra -s

TABLES = darm-tables.c
OBJECTS = main.o darm.o darm-tables.o

default: $(TABLES) main

main: $(OBJECTS)

$(TABLES): tblarmv7.py tablegen.py
	python tblarmv7.py

clean:
	rm -f $(OBJECTS) $(TABLES)
