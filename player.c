#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "game.h"

int most_cards_owner (Path* myPath, Player* p, Participant* pa) {
    int numberOfPlayers = pa->numberOfPlayers;
    int** cards = pa->cards;
    // int** sites = myPath->sites;
    int owner;

    int* cardsCollected = malloc(sizeof(int) * numberOfPlayers);

    for (int id = 0; id < numberOfPlayers; id++) {
        cardsCollected[id] = 0;
        for (int j = 0; j < NUM_A_TO_E; j++) {
            if (cards[id][j] > 0) {
                cardsCollected[id]++;
            }
        }
    }

    owner = find_max(cardsCollected, numberOfPlayers);

    return owner;
}

// Strategy
int next_move_b(Path* myPath, Player* p, Participant* pa) {
    int** sites = myPath->sites;
    int** positions = pa->positions;
    const int* moneys = pa->moneys;
    int id = p->playerId;
    int currentPos = pa->positions[id][1];
    const int nearestBarrier = nearest_barrier(myPath, &currentPos);
    int numberOfPlayersr = pa->numberOfPlayers;
    int nextMove;
    bool laterSite = true;

    int mostCardsOwner = most_cards_owner(myPath, p, pa);
    bool foundRi = false;
    bool foundV2 = false;
    bool foundOthers = false;

    // next site is empty
    if (pa->sizes[currentPos + 1] < sites[currentPos + 1][CAPACITY]) {
        // all of others players are on the later sites
        for (int i = 0; i < numberOfPlayersr && i!= id; i++) {
            if (positions[i][1] <= currentPos) {
                laterSite = false;
                break;
            }
        }
        if (laterSite == true) {
            return currentPos + 1;
        }
    }

    // other cases
    for (int i = currentPos + 2; i <= nearestBarrier; i++) {
        if (pa->sizes[i] < sites[i][CAPACITY]) {
            if (sites[i][SITE] == get_type_enum("Mo") &&
                    (moneys[id] % 2) != 0) {
                return i;
            }

            if (!foundRi && sites[i][SITE] == get_type_enum("Ri") &&
                    mostCardsOwner == id) {
                foundRi =true;
                nextMove = i;
            } else if (!foundRi && !foundV2 &&
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

int next_move_a(Path* myPath, Player* p, Participant* pa) {
    int** sites = myPath->sites;
    const int* id = &p->playerId;
    const int* moneys = pa->moneys;
    const int* currentPos = &pa->positions[*id][1];
    const int nearestBarrier = nearest_barrier(myPath, currentPos);
    int nextMove;

    bool foundMo = false;
    bool foundOthers = false;

    if (sites[*currentPos + 1][SITE] == get_type_enum("Mo") &&
            pa->sizes[*currentPos + 1] < sites[*currentPos + 1][CAPACITY]) {
        nextMove = *currentPos + 1;
        foundMo = true;
    }

    for (int i = *currentPos + 1; i <= nearestBarrier; i++) {
        if (pa->sizes[i] < sites[i][CAPACITY]) { // check empty space
            if (sites[i][SITE] == get_type_enum("Do")) {
                if ((moneys[*id] > 0)) {
                    return i;
                } else {
                    continue;
                }
            } else if (!foundMo && !foundOthers &&
                    sites[i][SITE] != get_type_enum("Ri")) {
                nextMove = i;
                foundOthers = true;
            }
        }
    }

    return nextMove;
}

// Handle input
bool get_hap(char* buffer, Path* myPath, Player* p, Participant* pa) {
    int size = sizeof("HAP");
    char tempStr[size];
    memcpy(tempStr, buffer, size - 1);
    tempStr[size - 1] = '\0';
    bool negativeMoneys = false;
    const int numOfLetters = 5;

    enum {
        receivedId = 0,     // p
        newPosition = 1,    // n
        addPoint = 2,       // s
        moneyChange = 3,    // m
        card = 4            // c
    };

    if (strcmp(tempStr, "HAP")) { //if three first chars are "HAP"
        return false;
    }

    // hapMessage does not inclus "HAP" in HAPp,n,s,m,c command
    char* hapMessage = buffer + (size - 1);  // buffer + 3
    char comma;
    int hapInfo[5];

    for (int i = 0; i < sizeof(hapInfo)/sizeof(int); i++) {
        if (sscanf(hapMessage, "%d", &hapInfo[i]) != 1) {
            return false;
        }
        hapMessage += count_number_digit(hapInfo[i]);
        sscanf(hapMessage, "%c", &comma);
        if (comma != ',') {
            return false;
        }
        hapMessage += sizeof(comma);
        // at third position, check whether next char is '-' or not
        if (i == addPoint && hapMessage[0] == '-') {
            hapMessage += sizeof(char);
            negativeMoneys = true;
        }
    }

    if (hapInfo[card] < 0 || hapInfo[card] > numOfLetters - 1) {
        return false;
    }

    int id = hapInfo[receivedId];
    pa->nextTurn = id; 
    pa->moneys[id] += negativeMoneys ? -hapInfo[moneyChange] :
            hapInfo[moneyChange];
    pa->points[id] += hapInfo[addPoint];

    handle_move(stderr, NULL, myPath, pa, id, hapInfo[newPosition]);
    return true;
}

void handle_input(Path* myPath, Player* p, Participant* pa,
        char playerType) {
    int defaultBufferSize = 20;
    char* buffer = malloc(sizeof(char) * defaultBufferSize);

    display_game(stderr, myPath, pa);

    while (true) {
        if (read_line(stdin, buffer, &defaultBufferSize)) {
            if (!strcmp(buffer, "YT")) {
                // return a move
                if (is_end_game(myPath, pa)) {
                    exit(handle_error_message(COMMUNICATION_PLAYER));
                }
                int nextMove = playerType == 'A' ? next_move_a(myPath, p, pa):
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
                if (!get_hap(buffer, myPath, p, pa)) {
                    exit(handle_player_errors(COMMUNICATION_PLAYER));
                }
            }
        } else {// cannot read handle input
            exit(handle_player_errors(COMMUNICATION_PLAYER));
        }
    }

    calc_scores(stderr, pa);
    free(buffer);
}

// Handle Path
bool get_path(Path* myPath, Player* p, Participant* pa) {
    printf("^");
    fflush(stdout);

    handle_path(stdin, myPath, &pa->numberOfPlayers);

    if (myPath->valid) {
        set_up(myPath, pa);
    }

    return myPath->valid;
}