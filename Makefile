CC = gcc
CFLAGS = -std=c99 -Wall -O2 -s

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

GENCODEC = armv7-tbl.c
GENCODEH = armv7-tbl.h

default: $(GENCODEC) $(GENCODEH) $(OBJ) tests\tests.exe

$(GENCODEC):
	python darmgen.py $@

$(GENCODEH):
	python darmgen.py $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%.exe: $(OBJ) $(GENCODEO)
	$(CC) $(CFLAGS) -o $@ $^
