AR = ar
CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra

# on non-windows, add -fPIC
ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC
endif

# on non-macosx, add -s
ifneq ($(shell uname),Darwin)
	CFLAGS += -s
endif

SRC = $(wildcard *.c tests/tests_thumb2.c)
OBJ = $(SRC:.c=.o)

GENCODESRC = darm-tbl.c darm-tbl.h armv7-tbl.c armv7-tbl.h \
	thumb-tbl.c thumb-tbl.h thumb2-tbl.c thumb2-tbl.h
GENCODEOBJ = darm-tbl.o armv7-tbl.o thumb-tbl.o thumb2-tbl.o

# generated stuff
GENR = $(GENCODESRC) $(GENCODEOBJ) $(OBJ)
LIBS  = libdarm.a libdarm.so
TOOLS = tests/tests.exe tests/expand.exe utils/elfdarm.exe

STUFF = $(GENR) $(LIBS) $(TOOLS)

default: $(STUFF)

$(GENCODESRC): darmgen.py darmtbl.py darmtbl2.py
	python darmgen.py

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%.exe: %.c
	$(CC) $(CFLAGS) -o $@ $^ libdarm.a -I. -Itests

%.so: $(OBJ) $(GENCODEOBJ)
	$(CC) -shared $(CFLAGS) -o $@ $^

%.a: $(OBJ) $(GENCODEOBJ)
	$(AR) cr $@ $^

test: $(STUFF)
	./tests/tests.exe

clean:
	rm -f $(STUFF)
