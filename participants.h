#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#define NUM_A_TO_E 5
// #define A 1
// #define B 2
// #define C 3
// #define D 4
// #define E 5

typedef struct participant {
    int numberOfPlayers;
    int nextTurn;
    int nextCard;
    int** positions;
    int* sizes; // 0 <= size <= CAPACITY of each site
    int* moneys;
    int* points;
    int* v1;
    int* v2;
    int** cards; //cards[playerId][letter]
    int* pointChange;
    int* moneyChange;
    int* nextMove;
} Participant;

#endif