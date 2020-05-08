#ifndef PLAYER_H
#define PLAYER_H

#include "path.h"
#include "participants.h"

typedef struct {
    int playerId;
    int currentPos;
} Player;

void errros_handler(int argc, char** argv, Path* myPath,
        Participant* pa, Player* p);
int most_cards_owner (Participant* pa, bool* noCardsFound);
int next_move_b(Path* myPath, Player* p, Participant* pa);
int next_move_a(Path* myPath, Player* p, Participant* pa);
bool get_path(Path* myPath, Player* p, Participant* pa);
void handle_input(Path* myPath, Player* p, Participant* pa,
        char playerType);

#endif