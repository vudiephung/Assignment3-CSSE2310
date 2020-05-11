#include "errors.h"
#include <stdio.h>

Error handle_error_message(Error type) {
    const char* errorMessage = "";
    switch (type) {
        case OK:
            return OK;
        case NUMS_OF_ARGS_DEALER:
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

Error handle_player_errors(Error type) {
    const char* errorMessage = "";
    switch (type) {
        case OK:
            return OK;
        case NUMS_OF_ARGS_PLAYER:
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