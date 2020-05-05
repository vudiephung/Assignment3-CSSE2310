#ifndef GAME_H
#define GAME_H

#include "deck.h"
#include "path.h"
#include "errors.h"
#include "participants.h"
#include "utils.h"

void set_up(Path* myPath, Participant* p);

void handle_move(FILE* file,Path* myPath, Participant* p, int playerId,
        const int toPosition);

void display_sites(FILE* file, Path* myPath);

void display_player_position(FILE* file, Path* myPath, Participant* p);

void display_dealer_output(FILE* file, Path* myPath, Participant* p);

void display_game(FILE* file, Path* myPath, Participant* p);

bool is_end_game(Path* myPath, Participant* pa);

void calc_scores(FILE* file, Participant* pa);

void initial_game(Deck* myDeck, Path* myPath, Participant* p, char** argv);

#endif