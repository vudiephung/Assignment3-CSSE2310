#include "path.h"
#include "deck.h"
#include "participants.h"
#include "errors.h"
#include "game.h"
#include "signal.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

// bool receivedSighub = false;

// void sighub_handler(int s) {
//     receivedSighub = true;
//     kill(-1, SIGKILL);
//     waitpid(-1, NULL, WNOHANG);
//     exit(handle_error_message(COMMUNICATION));
// }

void clean_up(void) { //
    return;
}

int main(int argc, char** argv){
    // struct sigaction sa;
    // memset(&sa, 0, sizeof(struct sigaction));
    // sa.sa_handler = sighub_handler;
    // sa.sa_flags = SA_RESTART;
    // sigaction(SIGHUP, &sa, 0);

    if (argc < 4) {
        return handle_error_message(NUMS_OF_ARGS_DEALER);
    }

    Deck* myDeck = malloc(sizeof(Deck) * 1);
    Path* myPath = malloc(sizeof(Path) * 1);
    Participant* p = malloc(sizeof(Participant) * 1);
    const int numberOfPlayers = argc - 3;
    p->numberOfPlayers = numberOfPlayers;   // 3 is number of agrs that
                                            // are not player

    if (!(is_valid_deck_file(argv[1], myDeck))) {
        return handle_error_message(DECK);
    }

    if(!(is_valid_path_file(argv[2], myPath, &numberOfPlayers))) {
        return handle_error_message(PATH);
    }

    run_game(myDeck, myPath, p, argv);

    clean_up();
    // remember to free myDeck, cards, sites, myPath
    return 0;
}
