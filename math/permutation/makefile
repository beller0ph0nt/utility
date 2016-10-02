OBJ = main.o Permutation.o

CC = $(CROSS_COMPILE)g++

INCLUDE_PATH = -I.

.PHONY: all clear

all: permutation

permutation: $(OBJ)
	$(CC) -o $@ $^
%.o: %.cpp
	$(CC) -c $(INCLUDE_PATH) -o $@ $<
clear:
	rm -f *.o permutation