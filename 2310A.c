#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "path.h"
#include "player.h"
#include "participants.h"

int main(int argc, char** argv) {
    Path* myPath = malloc(sizeof(Path) * 1);
    Player* p = malloc(sizeof(Player) * 1);
    Participant* pa = malloc(sizeof(Participant) * 1);

    errros_handler(argc, argv, myPath, pa, p);
    handle_input(myPath, p, pa, 'A');

    free(myPath);
    free(p);
    free(pa);
    return 0;
}