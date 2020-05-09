CFLAGS = -Wall -lm -pedantic -g -std=gnu99
.DEFAULT_GOAL := all

all: ass3 a b
a: a
b: b

ass3: utils.o errors.o game.o deck.o path.o 2310dealer.o
	gcc $(CFLAGS) -o 2310dealer errors.o utils.o deck.o path.o  2310dealer.o game.o

a: utils.o errors.o game.o deck.o path.o player.o 2310A.o
	gcc $(CFLAGS) -o 2310A errors.o utils.o game.o deck.o path.o player.o 2310A.o

b: utils.o errors.o game.o deck.o path.o player.o 2310B.o
	gcc $(CFLAGS) -o 2310B errors.o utils.o game.o deck.o path.o player.o 2310B.o

errors.o: errors.c
	gcc $(CFLAGS) -c errors.c

deck.o: deck.c
	gcc $(CFLAGS) -c deck.c

path.o: path.c
	gcc $(CFLAGS) -c path.c

game.o: game.c
	gcc $(CFLAGS) -c game.c

2310dealer.o: 2310dealer.c
	gcc $(CFLAGS) -c 2310dealer.c

2310A.o: 2310A.c
	gcc $(CFLAGS) -c 2310A.c

2310B.o: 2310B.c
	gcc $(CFLAGS) -c 2310B.c

player.o: player.c
	gcc $(CFLAGS) -c player.c

utils.o: utils.c
	gcc $(CFLAGS) -c utils.c

clean: 
	rm *.o 2310dealer 2310A 2310B
