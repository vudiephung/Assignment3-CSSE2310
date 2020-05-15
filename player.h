#ifndef PLAYER_H
#define PLAYER_H

#include "path.h"
#include "participants.h"

// For each player, the information about playerId and their current position
// (in which site?) should be saved
typedef struct {
    int playerId;
    int currentPos;
} Player;

// This header file is used for both 2310A.c and 2310B.c

void errros_handler(int argc, char** argv, Path* myPath,
        Participant* pa, Player* p);

int most_cards_owner(Participant* pa, bool* noCardsFound);

int strategy_b_others(Path* myPath, Participant* pa, int id);

int next_move_b(Path* myPath, Player* p, Participant* pa);

int next_move_a(Path* myPath, Player* p, Participant* pa);

bool get_hap(int* hapInfo, int arrayLength, char* hapMessage, int addPoint,
        bool* negativeMoneys);

bool handle_hap(char* buffer, Path* myPath, Participant* pa);

void handle_input(Path* myPath, Player* p, Participant* pa,
        char playerType);

bool get_path(Path* myPath, Player* p, Participant* pa);

#endif