#ifndef GAME_H
#define GAME_H

#include "deck.h"
#include "path.h"
#include "errors.h"
#include "participants.h"
#include "utils.h"

void set_up(Path* myPath, Participant* p);

void handle_move(FILE* file, Deck* myDeck, Path* myPath, Participant* p,
        int playerId, const int toPosition);

void display_sites(FILE* file, Path* myPath);

void display_player_position(FILE* file, Path* myPath, Participant* p);

void display_dealer_output(FILE* file, Path* myPath, Participant* p);

void display_game(FILE* file, Path* myPath, Participant* p);

bool is_end_game(Path* myPath, Participant* pa);

void calc_scores(FILE* file, Participant* pa);

void close_pipes_and_files (int id, int** pipesWrite, int** pipesRead,
        FILE** writeFile, FILE** readFile);

void send_last_message(pid_t* childIds, int numberOfPlayers, 
        FILE** writeFile, FILE** readFile, int** pipesWrite, int** pipesRead,
        bool early);

void initial_game(int numberOfPlayers, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead, pid_t* childIds, char* rawFile,
        char** argv);

void communicate(Deck* myDeck, Path* myPath, Participant* pa, pid_t* childIds,
        FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead);

void run_game(Deck* myDeck, Path* myPath, Participant* pa, char** argv);

#endif