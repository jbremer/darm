CC = gcc
CFLAGS = -std=c99 -Wall -O2 -s

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

GENCODEC = armv7-tbl.c
GENCODEH = armv7-tbl.h
GENCODEO = armv7-tbl.o

STUFF = $(GENCODEC) $(GENCODEH) $(OBJ) tests/tests.exe libdarm.a libdarm.so

default: $(STUFF)

$(GENCODEC):
	python darmgen.py $@

$(GENCODEH):
	python darmgen.py $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%.exe: %.c $(OBJ) $(GENCODEO)
	$(CC) $(CFLAGS) -o $@ $^

%.so: $(OBJ) $(GENCODEO)
	gcc -shared -o $@ $^

%.a: $(OBJ) $(GENCODEO)
	ar cr $@ $^

test: $(STUFF)
	./tests/tests.exe

clean:
	rm -f $(STUFF)
