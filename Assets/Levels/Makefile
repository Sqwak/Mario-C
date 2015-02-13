CC=gcc
CCFLAGS=-I, -Wall -02
LDFLAGS=-L/usr/lib -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_gfx
DEPS = header_file.h Makefile

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

all: lesson1 lesson2 lesson36 battleship \
	lesson38 sdl-example2 sdl-example2a

clean:
	rm *.o lesson1 lesson2 lesson36

lesson1: lesson1.o
	$(CC) $(LDFLAGS) -o lesson1 lesson1.o -I.

lesson2: lesson2.o
	$(CC) $(LDFLAGS) -o lesson2 lesson2.o -I.

lesson38: lesson38.o
	$(CC) $(LDFLAGS) -o lesson38 lesson38.o -I.

lesson40: lesson40.o
	$(CC) $(LDFLAGS) -o lesson40 lesson40.o -I.

sdl-example2a: sdl-example2a.o
	$(CC) $(LDFLAGS) -o sdl-example2a sdl-example2a.o -I.

battleship: battleship.o
	$(CC) $(LDFLAGS) -o battleship battleship.o -I.

clock: clock.o
	$(CC) $(LDFLAGS) -o clock clock.o -I.

foodgame: foodgame.o
	$(CC) $(LDFLAGS) -o foodgame foodgame.o -I.

flappybird: flappybird.o
	$(CC) $(LDFLAGS) -o flappybird flappybird.o -I.

space: space.o
	$(CC) $(LDFLAGS) -o space space.o -I.

pacman: pacman.o
	$(CC) $(LDFLAGS) -o pacman pacman.o -I.

gridmaker: gridmaker.o
	$(CC) $(LDFLAGS) -o gridmaker gridmaker.o -I.

mario: mario.o
	$(CC) $(LDFLAGS) -o mario mario.o -I.

marioleveleditor: marioleveleditor.o
	$(CC) $(LDFLAGS) -o marioleveleditor marioleveleditor.o -I.
