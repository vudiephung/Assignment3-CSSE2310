#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "game.h"


// Strategy
int next_move_a(Path* myPath, Player* p, Participant* pa) {
    int** sites = myPath->sites;
    const int* id = &p->playerId;
    const int* moneys = pa->moneys;
    const int* currentPos = &pa->positions[*id][1];
    const int nearestBarrier = nearest_barrier(myPath, currentPos);
    int nextMove;

    bool foundMo = false;
    bool foundOthers = false;

    for (int i = *currentPos + 1; i <= nearestBarrier; i++) {
        if (pa->sizes[i] < sites[i][CAPACITY]) { // check empty space
            if (sites[i][SITE] == get_type_enum("Do") && moneys[*id] > 0) {
                nextMove = i;
                return i;
            } else if (!foundMo && sites[i][SITE] == get_type_enum("Mo")) {
                nextMove = i;
                foundMo = true;
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
    char* hapMessage = buffer + (size - 1); 
    char comma;
    int hapInfo[5];

    for (int i = 0; i < sizeof(hapInfo)/sizeof(int); i++) {
        sscanf(hapMessage, "%d", &hapInfo[i]);
        hapMessage += count_number_digit(hapInfo[i]);
        sscanf(hapMessage, "%c", &comma);
        if (comma != ',') {
            // printf("Invalid comma at pos: %d\n", i);
            return false;
        }
        hapMessage += sizeof(comma);
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

    handle_move(stderr, myPath, pa, id, hapInfo[newPosition]);
    // printf("Moneys: %d Point: %d\n", pa->moneys, pa->points[id]);
    return true;
}

void handle_input(Path* myPath, Player* p, Participant* pa) {
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
                int nextMove = next_move_a(myPath, p, pa);
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
    // FILE* test = fopen("test", "w");
    printf("^");
    fflush(stdout);

    handle_path(stdin, myPath, &pa->numberOfPlayers);

    if (myPath->valid) {
        set_up(myPath, pa);
        // display_game(myPath);
    } else {
        // printf("Invlaid path\n");
    }

    return myPath->valid;
}