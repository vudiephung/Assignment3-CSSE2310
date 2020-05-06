#include "path.h"
#include "deck.h"
#include "participants.h"
#include "errors.h"
#include "game.h"
#include <stdlib.h>

void clean_up(void) { //
    return;
}

int main(int argc, char** argv){
    if (argc < 4) {
        return handle_error_message(NUMS_OF_ARGS_DEALER);
    }

    Deck* myDeck = malloc(sizeof(Deck) * 1);
    Path* myPath = malloc(sizeof(Path) * 1);
    Participant* p = malloc(sizeof(Participant) * 1);
    const int numberOfPlayers = argc - 3;
    p->numberOfPlayers = numberOfPlayers; // 3 is number of agrvs that
                                        // are not player

    if (!(is_valid_deck_file(argv[1], myDeck))) {
        return handle_error_message(DECK);
    }

    if(!(is_valid_path_file(argv[2], myPath, &numberOfPlayers))) {
        return handle_error_message(PATH);
    }

    initial_game(myDeck, myPath, p, argv);

    clean_up();
    // remember to free myDeck, cards, sites, myPath
    return 0;
}
