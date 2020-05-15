#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "path.h"
#include "deck.h"
#include "participants.h"
#include "errors.h"
#include "game.h"

// function handler when receive SIGHUB
void sighub_handler(int s) {
    kill(-getpid(), SIGKILL);
    wait(NULL);
}

int main(int argc, char** argv) {
    struct sigaction sighubAction;
    memset(&sighubAction, 0, sizeof(struct sigaction));
    sighubAction.sa_handler = sighub_handler;
    sighubAction.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sighubAction, 0);

    if (argc < 4) {
        return handle_error_message(NUMS_OF_ARGS);
    }

    Deck* myDeck = malloc(sizeof(Deck) * 1);
    Path* myPath = malloc(sizeof(Path) * 1);
    Participant* participants = malloc(sizeof(Participant) * 1);
    const int numberOfPlayers = argc - 3;   // 3 is number of agrs that
                                            // are not player
    participants->numberOfPlayers = numberOfPlayers;

    if (!(is_valid_deck_file(argv[1], myDeck))) {
        return handle_error_message(DECK);
    }

    if(!(is_valid_path(argv[2], myPath, numberOfPlayers))) {
        return handle_error_message(PATH);
    }

    run_game(myDeck, myPath, participants, argv);

    return 0;
}
