CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow
CFLAGS += -Ilib/stb
LDFLAGS = -lm

SRC = src/main.c
BIN = imgcmp

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(BIN)

clean:
	-rm $(BIN)
