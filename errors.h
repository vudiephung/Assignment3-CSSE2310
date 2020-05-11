#ifndef ERROR_H
#define ERROR_H

typedef enum {
    OK = 0,
    NUMS_OF_ARGS_PLAYER = 1,
    NUMS_OF_ARGS_DEALER = 1,
    PLAYER_COUNT = 2,
    DECK = 2,
    ID = 3,
    PATH = 3,
    STARTING_PROCESS = 4,
    PATH_PLAYER = 4,
    COMMUNICATION = 5,
    EARLY_OVER = 5,
    COMMUNICATION_PLAYER = 6
} Error;

// Base on the error type, this function will fprintf() the corresponding
// message to stderr and return Error
// e.g: handle_error_message(NUMS_OF_ARGS_PLAYER)
// will fprintf to stderr the message "Usage: 2310dealer deck path p1 {p2}"
// and return 1 (NUMS_OF_ARGS_PLAYER)
Error handle_error_message(Error type);

// Mostly like handle_error_message() but with different error messages
// for Players
Error handle_player_errors(Error type);

#endif