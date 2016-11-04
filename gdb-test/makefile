CC=gcc

.PHONY: all

all: libso.so bin1 bin2

libso.so: so.c so.h
	$(CC) -ggdb -fPIC -shared -o $@ $^
bin1: bin1.c
	$(CC) -ggdb -o $@ $^ -L. -lso
bin2: bin2.c
	$(CC) -ggdb -o $@ $^ -L. -lso
clear:
	rm -f *.o libso.so bin1 bin2