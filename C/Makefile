CC=gcc
CCFLAGS=-I, -Wall -02
LDFLAGS=-L/usr/lib -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_gfx
DEPS = header_file.h Makefile

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

all: mario

clean:
	rm *.o mario

mario: main.o
	$(CC) $(LDFLAGS) -o mario main.o -I.
