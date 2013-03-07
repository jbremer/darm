CC = gcc
CFLAGS = -std=c99 -Wall -O2 -s -Wextra

ifneq ($(OS),Windows_NT)
	PIC_FLAGS = -fPIC
endif

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

GENCODESRC = armv7-tbl.c armv7-tbl.h
GENCODEOBJ = armv7-tbl.o

STUFF = $(GENCODESRC) $(GENCODEOBJ) $(OBJ) \
	tests/tests.exe libdarm.a libdarm.so

default: $(STUFF)

$(GENCODESRC):
	python darmgen.py $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^ $(PIC_FLAGS)

%.exe: %.c $(OBJ) $(GENCODEOBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.so: $(OBJ) $(GENCODEOBJ)
	gcc -shared $(CFLAGS) -o $@ $^

%.a: $(OBJ) $(GENCODEOBJ)
	ar cr $@ $^

test: $(STUFF)
	./tests/tests.exe

clean:
	rm -f $(STUFF)
