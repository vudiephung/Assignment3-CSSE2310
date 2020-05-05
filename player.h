#ifndef PLAYER_H
#define PLAYER_H

#include "path.h"
#include "participants.h"

typedef struct {
    int playerId;
    int currentPos;
} Player;

// void move(Path* myPath, Participant* pa, int playerId,
//         const int toPosition) {
int next_move_a(Path* myPath, Player* p, Participant* pa);
bool get_path(Path* myPath, Player* p, Participant* pa);
void handle_input(Path* myPath, Player* p, Participant* pa);

#endif