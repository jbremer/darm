AR = ar
CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra -Wno-missing-field-initializers

# on non-windows, add -fPIC
ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC
	BIN_EXT =
	LIB_EXT = .so
endif

ifeq ($(OS),Windows_NT)
	BIN_EXT = .exe
	LIB_EXT = .dll
endif

# on non-macosx, add -s
ifneq ($(shell uname),Darwin)
	CFLAGS += -s
endif

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

GENCODESRC = darm-tbl.c darm-tbl.h armv7-tbl.c armv7-tbl.h \
	thumb-tbl.c thumb-tbl.h thumb2-tbl.c thumb2-tbl.h
GENCODEOBJ = darm-tbl.o armv7-tbl.o thumb-tbl.o thumb2-tbl.o

# generated stuff
GENR = $(GENCODESRC) $(GENCODEOBJ) $(OBJ)
LIBS  = libdarm.a libdarm$(LIB_EXT)
TOOLS = tests/tests$(BIN_EXT) tests/expand$(BIN_EXT) utils/elfdarm$(BIN_EXT)

STUFF = $(GENR) $(LIBS) $(TOOLS)

default: $(STUFF)

$(GENCODESRC): darmgen.py darmtbl.py darmtbl2.py
	python darmgen.py

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%$(BIN_EXT): %.c
	$(CC) $(CFLAGS) -o $@ $^ libdarm.a -I. -Itests

%$(LIB_EXT): $(OBJ) $(GENCODEOBJ)
	$(CC) -shared $(CFLAGS) -o $@ $^

%.a: $(OBJ) $(GENCODEOBJ)
	$(AR) cr $@ $^

test: $(STUFF)
	./tests/tests$(BIN_EXT)

clean:
	rm -f $(STUFF)
