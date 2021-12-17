CC = gcc
RM = rm -rf
EXEC = 2048
SRC = 2048.c
LDFLAGS = `sdl-config --cflags --libs`
LIBS = SDL_ttf

all: game

game:
	$(CC) $(SRC) -o $(EXEC) $(LDFLAGS) -l $(LIBS)

clean:
	$(RM) 2048