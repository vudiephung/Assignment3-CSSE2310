<<<<<<< HEAD
CFLAGS = -Wall -Werror -lm -pedantic -g -std=gnu99
=======
CFLAGS = -Wall -Werror -pedantic -g -std=gnu99
>>>>>>> 5d7b2582604f2e4fbf681bac8dc58dc4437c3ba7
.DEFAULT_GOAL := a

all: ass3 a
a: a

ass3: utils.o errors.o game.o deck.o path.o 2310dealer.o
	gcc $(CFLAGS) -o 2310dealer errors.o utils.o deck.o path.o  2310dealer.o game.o

a: utils.o errors.o game.o participants.o path.o player.o 2310A.o
	gcc $(CFLAGS) -o 2310A errors.o utils.o game.o path.o player.o 2310A.o

errors.o: errors.c
	gcc $(CFLAGS) -c errors.c

participants.o: participants.c
	gcc $(CFLAGS) -c participants.c

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

player.o: player.c
	gcc $(CFLAGS) -c player.c

utils.o: utils.c
	gcc $(CFLAGS) -c utils.c

clean: 
	rm *.o 2310dealer 2310A 2310B
