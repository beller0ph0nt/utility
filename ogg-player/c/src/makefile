PLAYER_OBJ = ogg-player.o \
             wrap-ogg.o \
             wrap-theora.o \
             ogg-skeleton.o

CFLAGS = -Wall -DDEBUG

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

LIB = -logg -ltheora -lvorbis

.PHONY: all

all: ogg-player

ogg-player: $(PLAYER_OBJ)
	$(CC) -o $@ $^ $(LIB) $(LIBPATH)

ogg-player.o: ogg-player.c
	$(CC) -c $(CFLAGS) -o $@ $^
wrap-ogg.o: wrap-ogg.c
	$(CC) -c $(CFLAGS) -o $@ $^
wrap-theora.o: wrap-theora.c
	$(CC) -c $(CFLAGS) -o $@ $^
ogg-skeleton.o: ogg-skeleton.c
	$(CC) -c $(CFLAGS) -o $@ $^

clear:
	rm -f *.o ogg-player