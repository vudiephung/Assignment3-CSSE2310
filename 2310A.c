#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"
#include "path.h"
#include "player.h"
#include "participants.h"
#include "utils.h"

int main (int argc, char** argv) {
    if (argc != 3) {
        return handle_player_errors(NUMS_OF_ARGS_PLAYER);
    }

    int numberOfPlayers;
    if (!is_digits_only(argv[1], &numberOfPlayers) || numberOfPlayers < 1) {
        return handle_player_errors(PLAYER_COUNT);
    }

    int playerId;
    if (!is_digits_only(argv[2], &playerId) || playerId < 0 ||
            playerId >= numberOfPlayers) {
        return handle_player_errors(ID);
    }

    Path* myPath = malloc(sizeof(Path) * 1);
    Player* p = malloc(sizeof(Player) * 1);
    Participant* pa = malloc(sizeof(Participant) * 1);
    // Set up Player
    pa->numberOfPlayers = numberOfPlayers;
    p->playerId = playerId;

    if (!get_path(myPath, p, pa)) {
        return handle_player_errors(PATH_PLAYER);
    }

    handle_input(myPath, p, pa, 'A');

    free(myPath);
    free(p);
    return 0;
}