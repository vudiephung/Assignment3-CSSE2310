#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "path.h"
#include "player.h"
#include "participants.h"

int main(int argc, char** argv) {
    Path* myPath = malloc(sizeof(Path) * 1);
    Player* player = malloc(sizeof(Player) * 1);
    Participant* participants = malloc(sizeof(Participant) * 1);

    errros_handler(argc, argv, myPath, participants, player);
    handle_input(myPath, player, participants, 'B');

    free(myPath);
    free(player);
    free(participants);
    return 0;
}