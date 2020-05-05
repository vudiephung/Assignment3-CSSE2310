#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#define NUM_A_TO_E 5
// #define A 0
// #define B 1
// #define C 2
// #define D 3
// #define E 4

typedef struct participant {
    int numberOfPlayers;
    int nextTurn;
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