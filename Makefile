AR = ar
CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Wextra

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

AUTO = darm-tables.c darm-tables.h darm-instr.h
OBJS = main.o darm.o darm-tables.o libdarm.a
BINS = libdarm$(LIB_EXT) main$(BIN_EXT)

default: $(AUTO) $(OBJS) $(BINS)

libdarm$(LIB_EXT): darm.o darm-tables.o
	$(CC) -shared $(CFLAGS) -o $@ $^

libdarm.a: darm.o darm-tables.o
	$(AR) cr $@ $^

main$(BIN_EXT): main.o libdarm.a
	$(CC) $(CFLAGS) -o $@ $^

$(AUTO): generator.py tablegen.py tables
	python generator.py

clean:
	rm -f $(AUTO) $(OBJS) $(BINS)
