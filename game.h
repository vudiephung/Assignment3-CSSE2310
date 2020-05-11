#ifndef GAME_H
#define GAME_H

#include <sys/types.h>
#include <unistd.h>
#include "deck.h"
#include "path.h"
#include "errors.h"
#include "participants.h"
#include "utils.h"

// With this function, dealer can determines which player should go next
// based on their current position (get from struct Participant)
// and the number of sites (get from path)
// Then save the next turn (e.g: 0) into pa->nextTurn
void calc_next_turn(Path* myPath, Participant* pa);

// Malloc most of variables of struct Participant with needed sizes
// and set up default values. For example, at the beginning of the game, all
// of the players at positions 0 with id from high to low; points and money of
// players are 0 and 7 respectively, etc
void set_up(Path* myPath, Participant* p);

// This function is used for both dealer and player, based on given File* file
// to handle with playerId move to which position
// if dealer: file == stdout, dealer will check the type of the
// next site (toPosition) and handle with it. (e.g: +3 money if next site is 
// Mo)
// If player: file == stderr, are allowed to update their V1 or V2 status
// For dealer and players:
// Increase and decrease the capacity of their old position and new position
// respectively and display to the screen the output from dealer, sites and
// new position after the move 
void handle_move(FILE* file, Deck* myDeck, Path* myPath, Participant* pa,
        int playerId, const int toPosition);

// Used for both dealer or player (stdout or stderr)
// Display to the screen the sites (e.g: :: Mo Do Ri ::)
// based on the sites and numberOfSites taken from struct Path
void display_sites(FILE* file, Path* myPath);

// Based on the positions of players stored in pa->positions, displayer to the
// screen with right orders
void display_player_position(FILE* file, Path* myPath, Participant* pa);

// Display output from dealer. e.g: Player 0 Money=7 V1=0 .....
void display_dealer_output(FILE* file, Path* myPath, Participant* pa);

// display_game() includes display_sites() and display_player_position()
void display_game(FILE* file, Path* myPath, Participant* pa);

// Return true iff all players are at the last barrier and false otherwise
bool is_end_game(Path* myPath, Participant* pa);

// Calculate scores and then display the message to the screen
// e.g: "Scores: 10,12\n"
void calc_scores(FILE* file, Participant* pa);

// Close the files and pipes that are left based on the id of the
// players
void close_pipes_and_files(int id, int** pipesWrite, int** pipesRead,
        FILE** writeFile, FILE** readFile);

// Send "DONE" to all of the players, or "EARLY" to players that are not
// terminated by signal based on the parameter bool early
// After send to the pipes, close files and pipes of each players
// kill and reap the "alive" players in case of the players are not exit
// if early == true, exit dealer with Comms error
void send_last_message(pid_t* childIds, int numberOfPlayers, 
        FILE** writeFile, FILE** readFile, int** pipesWrite, int** pipesRead,
        bool early);

// After fork(), each player will close WRITE_END of its pipesWrite and
// READ_END of its pipesRead based on its id, then dup2() and execlp
// with given char* currentPlayer (e.g: "./2310A"), char* playerCountStr
// (e.g: "4") and its id as a string (e.g: "0")
void handle_child(int id, char* currentPlayer, char* playersCountString,
        FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead);

// The dealer (parent) will run this function, close READ_END and WRITE_END of
// each pipes, fdopen() and read caret '^' from player and
// send the path char* rawPath to the pipes with given id in parameter
// if it doesnt get the '^' character, exit with starting process error code
void handle_parent(int id, char* rawPath, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead);

// For each player in numberOfPlayers, malloc pipesRead and pipesWrite with
// size of 2 int, fork() and save id of each childs in pid_t*
// run handle_child() and handle_parent()
// From argv, get "./2310A" for example and pass down to handle_child()
void initial_game(int numberOfPlayers, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead, pid_t* childIds, char* rawPath,
        char** argv);

// After fork() and send Path to all parents
// In this function, display_game(), while the game is not end, determines
// which player should go next. To avoid SIGPIPE, check whether that player
// is alive or not before sending "YT", read "DO", exit if comms error with
// that player require invalid move. Otherwisem handle_move() and send "HAP"
// to all players 
void communicate(Deck* myDeck, Path* myPath, Participant* pa, pid_t* childIds,
        FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead);

// set up sigaction of SIGCHILD, set up variables and runs the game
// When the game is over, send "DONE" to all players
void run_game(Deck* myDeck, Path* myPath, Participant* pa, char** argv);

#endif