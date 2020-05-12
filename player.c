#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "game.h"

void errros_handler(int argc, char** argv, Path* myPath,
        Participant* pa, Player* p) {
    if (argc != 3) {
        exit(handle_player_errors(NUMS_OF_ARGS));
    }

    int numberOfPlayers;
    if (!is_digits_only(argv[1], &numberOfPlayers) || numberOfPlayers < 1) {
        exit(handle_player_errors(PLAYER_COUNT));
    }
    pa->numberOfPlayers = numberOfPlayers;

    int playerId;
    if (!is_digits_only(argv[2], &playerId) || playerId < 0 ||
            playerId >= numberOfPlayers) {
        exit(handle_player_errors(ID));
    }
    p->playerId = playerId;

    if (!get_path(myPath, p, pa)) {
        exit(handle_player_errors(PATH_PLAYER));
    }
}

// Helper function of most_cards_owner() to find one player who has most of
// the cards, return -1 iff there are at least 2 players have the same highest
// number of cards
int find_index_of_max(int* array, int size) {
    int max = array[0];
    int index = 0;
    for (int i = 1; i < size; i++) {
        if (array[i] == max) {
            if (max != 0) {
                index = -1;
            } else {
                index = i;
            }
        } else if (array[i] > max) {
            max = array[i];
            index = i;
        }
    }
    return index;
}

int most_cards_owner(Participant* pa, bool* noCardsFound) {
    int numberOfPlayers = pa->numberOfPlayers;
    int** cards = pa->cards;
    int owner;

    int* cardsCollected = malloc(sizeof(int) * numberOfPlayers);

    for (int id = 0; id < numberOfPlayers; id++) {
        cardsCollected[id] = 0;
        for (int j = 0; j < NUM_A_TO_E; j++) {
            if (cards[id][j] > 0) {
                *noCardsFound = false;
                cardsCollected[id] += cards[id][j];
            }
        }
    }

    owner = find_index_of_max(cardsCollected, numberOfPlayers);

    free(cardsCollected);
    return owner;
}

int strategy_b_others(Path* myPath, Participant* pa, int id) {
    // other cases
    int currentPos = pa->positions[id][1];
    int nearestBarrier = nearest_barrier(myPath, currentPos);
    int** sites = myPath->sites;
    const int* moneys = pa->moneys;
    int nextMove;

    bool noCardsFound = true;
    int mostCardsOwner = most_cards_owner(pa, &noCardsFound);
    bool foundRi = false;
    bool foundV2 = false;
    bool foundOthers = false;

    for (int i = currentPos + 1; i <= nearestBarrier; i++) {
        if (pa->sizes[i] < sites[i][CAPACITY]) {
            if (sites[i][SITE] == get_type_enum("Mo") &&
                    (moneys[id] % 2) != 0) {
                return i;
            }
            if (!foundRi && sites[i][SITE] == get_type_enum("Ri") &&
                    (noCardsFound || mostCardsOwner == id)) {
                foundRi = true;
                nextMove = i;
                noCardsFound = false;
            }
            if (!foundRi && !foundV2 &&
                    sites[i][SITE] == get_type_enum("V2")) {
                foundV2 = true;
                nextMove = i;
            } else if (!foundRi && !foundV2 && !foundOthers) {
                foundOthers = true;
                nextMove = i;
            }
        }
    }

    return nextMove;
}

// Strategy B
int next_move_b(Path* myPath, Player* p, Participant* pa) {
    int** sites = myPath->sites;
    int** positions = pa->positions;
    int id = p->playerId;
    int currentPos = pa->positions[id][1];
    bool laterSite = true;

    // next site is not full
    if (pa->sizes[currentPos + 1] < sites[currentPos + 1][CAPACITY]) {
        // all of others players are on the later sites
        for (int i = 0; i < pa->numberOfPlayers; i++) {
            if (i != id && positions[i][1] <= currentPos) {
                laterSite = false;
                break;
            }
        }
        if (laterSite == true) {
            return currentPos + 1;
        }
    }

    return strategy_b_others(myPath, pa, id);
}

// Strategy A
int next_move_a(Path* myPath, Player* p, Participant* pa) {
    int** sites = myPath->sites;
    const int* id = &p->playerId;
    const int* moneys = pa->moneys;
    int currentPos = pa->positions[*id][1];
    int nearestBarrier = nearest_barrier(myPath, currentPos);
    int nextMove;

    bool foundMo = false;
    bool foundOthers = false;

    if (sites[currentPos + 1][SITE] == get_type_enum("Mo") &&
            pa->sizes[currentPos + 1] < sites[currentPos + 1][CAPACITY]) {
        nextMove = currentPos + 1;
        foundMo = true;
    }

    for (int i = currentPos + 1; i <= nearestBarrier; i++) {
        if (pa->sizes[i] < sites[i][CAPACITY]) { // check empty space
            if (sites[i][SITE] == get_type_enum("Do")) {
                if ((moneys[*id] > 0)) {
                    return i;
                } else {
                    continue;
                }
            } else if (!foundMo && !foundOthers &&
                    sites[i][SITE] != get_type_enum("Ri") &&
                    sites[i][SITE] != get_type_enum("Mo")) {
                nextMove = i;
                foundOthers = true;
            }
        }
    }

    return nextMove;
}

bool get_hap(int* hapInfo, int arrayLength, char* hapMessage, int addPoint,
        bool* negativeMoneys) {
    char comma;
    for (int i = 0; i < arrayLength; i++) {
        if (sscanf(hapMessage, "%d", &hapInfo[i]) != 1) {
            return false;
        }
        hapMessage += count_number_digit(hapInfo[i]);
        sscanf(hapMessage, "%c", &comma);
        if (comma != ',') {
            return false;
        }
        hapMessage += sizeof(comma);
        // at third position, check whether next char is minus (-) or not
        if (i == addPoint && hapMessage[0] == '-') {
            hapMessage += sizeof(char);
            *negativeMoneys = true;
        }
    }
    return true;
}

bool handle_hap(char* buffer, Path* myPath, Player* p, Participant* pa) {
    int size = sizeof("HAP");
    char temporaryString[size];
    memcpy(temporaryString, buffer, size - 1);
    temporaryString[size - 1] = '\0';
    bool negativeMoneys = false;
    const int numberOfLetters = 5;

    const int receivedId = 0;     // p
    const int newPosition = 1;    // n
    const int addPoint = 2;       // s
    const int moneyChange = 3;    // m
    const int receivedCard = 4;   // c
    const int arrayLength = 5;    // p...c

    if (strcmp(temporaryString, "HAP")) { //if three first chars are "HAP"
        return false;
    }

    // hapMessage does not inclus "HAP" in HAPp,n,s,m,c command
    char* hapMessage = buffer + (size - 1);  // buffer + 3
    int hapInfo[arrayLength];

    if (!get_hap(hapInfo, arrayLength, hapMessage, addPoint,
            &negativeMoneys)) {
        return false;
    }

    int id = hapInfo[receivedId];
    int toPosition = hapInfo[newPosition];
    int card = hapInfo[receivedCard];

    if (id >= pa->numberOfPlayers ||
            toPosition >= myPath->numberOfSites ||
            card < 0 || card > numberOfLetters) {
        return false;
    }

    pa->nextTurn = id; 
    pa->moneys[id] += negativeMoneys ? -hapInfo[moneyChange] :
            hapInfo[moneyChange];
    pa->points[id] += hapInfo[addPoint];
    pa->cards[id][card - 1] += 1;

    handle_move(stderr, NULL, myPath, pa, id, toPosition);
    return true;
}

void handle_input(Path* myPath, Player* p, Participant* pa, char playerType) {
    int defaultBufferSize = 20;
    char* buffer = malloc(sizeof(char) * defaultBufferSize);

    display_game(stderr, myPath, pa);
    while (true) {
        if (read_line(stdin, buffer, &defaultBufferSize)) {
            if (!strcmp(buffer, "YT")) {
                // return a move
                int nextMove = playerType == 'A' ?
                        next_move_a(myPath, p, pa) :
                        next_move_b(myPath, p, pa);
                printf("DO%d\n", nextMove);
                fflush(stdout);
            } else if (!strcmp(buffer, "EARLY")) {
                // Game ended early due to error
                exit(handle_player_errors(EARLY_OVER));
            } else if (!strcmp(buffer, "DONE")) {
                // Game ened normally
                break;
            } else {
                if (!handle_hap(buffer, myPath, p, pa)) {
                    exit(handle_player_errors(COMMUNICATION_PLAYER));
                }
            }
        } else { // cannot read from stdin
            exit(handle_player_errors(COMMUNICATION_PLAYER));
        }
    }

    calc_scores(stderr, pa);
    free(buffer);
}

bool get_path(Path* myPath, Player* p, Participant* pa) {
    printf("^");
    fflush(stdout);

    handle_path(stdin, myPath, pa->numberOfPlayers);

    if (myPath->valid) {
        set_up(myPath, pa);
    }
    return myPath->valid;
}