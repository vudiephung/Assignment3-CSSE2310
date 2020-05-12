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

// Based on given argc and argv:
// 1. exit with error code of number of args if arg != 3
// 2. exit with error code of player count if argv[1] is invalid
// If argv[1] is valid, save it into pa->numberOfPlayers
// 3. Do the same thing for argv[2] and save into p->playerId if valid
// 4. read the path, if not valid, exit with error code of invalid path
void errros_handler(int argc, char** argv, Path* myPath,
        Participant* pa, Player* p);

// return the player id who has most cards
// return -1 if there are at least 2 players are most cards owner
// If there is at least one player has a card, *nocardsFound = false
int most_cards_owner(Participant* pa, bool* noCardsFound);

// Helper function for next_move_b() below, return the next position based
// on player B strategy (do not include the case
// "If the next site is not full and all other players are on later sites
// than us, move forward one site"
int strategy_b_others(Path* myPath, Participant* pa, int id);

// return the next position based on the strategy of Player B
int next_move_b(Path* myPath, Player* p, Participant* pa);

// return the next position based on the strategy of Player A
int next_move_a(Path* myPath, Player* p, Participant* pa);

// Args: hapInfo: array of type int saves the read information from HAP
// message. e.g: hapInfo[0] = p, hapInfo[1] = n .... hapInfo[4] = c
// arrayLength: legnth of hapInfo = 5;
// hapMessage: Message from HAP that does not contain "HAP"
// e.g: "HAP0,2,3,-7,0" -> hapMessage = "0,2,3,-7,0"
// Loop through hapMessage, at position addPoint (which is 2), if
// after the following comma ',' is minus '-', *negativeMoneys = true
// return true iff valid hap and false otherwise
bool get_hap(int* hapInfo, int arrayLength, char* hapMessage, int addPoint,
        bool* negativeMoneys);

// From the complete HAP message from buffer. e.g: "HAP0,2,3,-7,0",
// get 3 first chars and comapare with "HAP", return false if it doesn't match
// After get_hap(), save needed data into struct Participant* pa,
// handle the move and return true
bool handle_hap(char* buffer, Path* myPath, Player* p, Participant* pa);

// In this function, display the sites and positions of player
// Continuously get input from stdin and handle with it
// Print the score of the game if get "DONE" from stdin
void handle_input(Path* myPath, Player* p, Participant* pa,
        char playerType);

// print '^', handle the path. If path file is valid, set up the variables
// and return false if the path is invalid and otherwise
bool get_path(Path* myPath, Player* p, Participant* pa);

#endif