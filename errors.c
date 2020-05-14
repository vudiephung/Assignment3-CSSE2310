#include "errors.h"
#include <stdio.h>

// Base on the error type, this function will fprintf() the corresponding
// message to stderr and return Error
// e.g: handle_error_message(NUMS_OF_ARGS_PLAYER)
// will fprintf to stderr the message "Usage: 2310dealer deck path p1 {p2}"
// and return 1 (NUMS_OF_ARGS)
Error handle_error_message(Error type) {
    const char* errorMessage = "";
    switch (type) {
        case OK:
            return OK;
        case NUMS_OF_ARGS:
            errorMessage = "Usage: 2310dealer deck path p1 {p2}";
            break;
        case DECK:
            errorMessage = "Error reading deck";
            break;
        case PATH:
            errorMessage = "Error reading path";
            break;
        case STARTING_PROCESS:
            errorMessage = "Error starting process";
            break;
        case COMMUNICATION:
            errorMessage = "Communications error";
            break;
        default:
            break;
    }
    fprintf(stderr, "%s\n", errorMessage);
    return type;
}

// Mostly like handle_error_message() but with different error messages
// for Players
Error handle_player_errors(Error type) {
    const char* errorMessage = "";
    switch (type) {
        case OK:
            return OK;
        case NUMS_OF_ARGS:
            errorMessage = "Usage: player pcount ID";
            break;
        case PLAYER_COUNT:
            errorMessage = "Invalid player count";
            break;
        case ID:
            errorMessage = "Invalid ID";
            break;
        case PATH_PLAYER:
            errorMessage = "Invalid path";
            break;
        case EARLY_OVER:
            errorMessage = "Early game over";
            break;
        case COMMUNICATION_PLAYER:
            errorMessage = "Communications error";
            break;
        default:
            break;
    }
    fprintf(stderr, "%s\n", errorMessage);
    return type;
}