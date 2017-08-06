CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

INCLUDE_DIR = -I$(abspath ./include)

CFLAGS = -g -Wall -DDEBUG
TST_CFLAGS = -g -Wall

CLI_OBJ = src/cli.o src/cli-lib.o src/common-lib.o
CLI_BIN = build/cli

SRV_OBJ = src/srv.o src/common-lib.o
SRV_BIN = build/srv

TST_SRC = $(wildcard tests/*.c)
TST_OBJ = $(patsubst tests/%.c, tests/%.o, $(TST_SRC))
TST_BIN = $(patsubst tests/%.c, tests/%,   $(TST_SRC))

VPATH = src tests

.PHONY: all clear tests

all: $(CLI_BIN) $(SRV_BIN) tests

$(CLI_BIN): $(CLI_OBJ)
	$(CC) -o $@ $^ $(LIB) $(LIBPATH)

$(SRV_BIN): $(SRV_OBJ)
	$(CC) -o $@ $^ $(LIB) $(LIBPATH)

tests: $(TST_BIN)

tests/test_%: tests/test_%.o tests/cli-lib.o tests/common-lib.o tests/cli.o
	$(CC) -o $@ $^ -Wl,--wrap=read_resp
tests/%.o: src/%.c
	$(CC) -c $(TST_CFLAGS) $(INCLUDE_DIR) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE_DIR) -o $@ $^

clear:
	rm -f $(CLI_OBJ) $(CLI_BIN) $(SRV_OBJ) $(SRV_BIN) $(TST_OBJ) $(TST_BIN)