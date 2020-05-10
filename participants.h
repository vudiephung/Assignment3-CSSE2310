#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#define NUM_A_TO_E 5

typedef struct Participant {
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