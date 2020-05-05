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
} Errors;

Errors handle_error_message(Errors type);
Errors handle_player_errors(Errors type);

#endif