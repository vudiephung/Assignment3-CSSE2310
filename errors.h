#ifndef ERROR_H
#define ERROR_H

// Errors code for both dealer and player
typedef enum {
    OK = 0, // For Both dealer and player, return 0 if there is no error 
    NUMS_OF_ARGS = 1, // and return 1 if the number of arguments is not valid
    PLAYER_COUNT = 2, // dealer: Invalid player count
    DECK = 2, // dealer: Invalid deck file or contents
    ID = 3, // player: Invalid player ID
    PATH = 3, // dealer: Invalid path files or contents
    STARTING_PROCESS = 4, // player: Error starting process
    PATH_PLAYER = 4, // player: Error in path
    COMMUNICATION = 5, // dealer: Comms error
    EARLY_OVER = 5, // player: Early game over
    COMMUNICATION_PLAYER = 6 // player: Comms error
} Error;

Error handle_error_message(Error type);

Error handle_player_errors(Error type);

#endif