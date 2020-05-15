#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "game.h"

// Based on given argc and argv:
// 1. exit with error code of number of args if arg != 3
// 2. exit with error code of player count if argv[1] is invalid
// If argv[1] is valid, save it into
// numberOfPlayers of struct 'Participant'
// 3. Do the same thing for argv[2] and save into variable playerId
// of struct 'Player' if valid
// 4. read the path, if not valid, exit with error code of invalid path
// return void;
void errros_handler(int argc, char** argv, Path* myPath,
        Participant* participants, Player* player) {
    if (argc != 3) {
        exit(handle_player_errors(NUMS_OF_ARGS));
    }

    int numberOfPlayers;
    if (!is_digits_only(argv[1], &numberOfPlayers) || numberOfPlayers < 1) {
        exit(handle_player_errors(PLAYER_COUNT));
    }
    participants->numberOfPlayers = numberOfPlayers;

    int playerId;
    if (!is_digits_only(argv[2], &playerId) || playerId < 0 ||
            playerId >= numberOfPlayers) {
        exit(handle_player_errors(ID));
    }
    player->playerId = playerId;

    if (!get_path(myPath, player, participants)) {
        exit(handle_player_errors(PATH_PLAYER));
    }
}

// Helper function of most_cards_owner() to find one player who has most of
// the cards based on the 'array' of total cards collected by each player
// and the 'size' of that array, which is numberOfPlayers.
// Return -1 iff there are at least 2 players have the same highest
// number of cards. Otherwise, return playerId who is most cards owner
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

// get 'cards' and 'numberOfPlayers' from struct 'Participant'
// return the player id who has most cards
// return -1 if there are at least 2 players are most cards owner
// If there is at least one player has a card, *nocardsFound = false
int most_cards_owner(Participant* participants, bool* noCardsFound) {
    int numberOfPlayers = participants->numberOfPlayers;
    int** cards = participants->cards;
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

// Helper function for next_move_b() below, return the next position based
// on player B strategy (do not include the case
// "If the next site is not full and all other players are on later sites
// than us, move forward one site"
// Parameters: get sites from 'myPath', and other needed information
// for strategy such as moneys from struct 'Participant'
// and the 'id' of currnet player
int strategy_b_others(Path* myPath, Participant* participants, int id) {
    // other cases
    int currentPos = participants->positions[id][1];
    int nearestBarrier = nearest_barrier(myPath, currentPos);
    int** sites = myPath->sites;
    const int* moneys = participants->moneys;
    int nextMove;

    bool noCardsFound = true;
    int mostCardsOwner = most_cards_owner(participants, &noCardsFound);
    bool foundRi = false;
    bool foundV2 = false;
    bool foundOthers = false;

    for (int i = currentPos + 1; i <= nearestBarrier; i++) {
        if (participants->sizes[i] < sites[i][CAPACITY]) {
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
// return the next position based on the strategy of Player B
// by gettings sites from strcut 'Path', player id from struct 'Player'
// and positions from struct 'Participants'
int next_move_b(Path* myPath, Player* player, Participant* participants) {
    int** sites = myPath->sites;
    int** positions = participants->positions;
    int id = player->playerId;
    int currentPos = participants->positions[id][1];
    bool laterSite = true;

    // next site is not full
    if (participants->sizes[currentPos + 1] < sites[currentPos + 1][CAPACITY]) {
        // all of others players are on the later sites
        for (int i = 0; i < participants->numberOfPlayers; i++) {
            if (i != id && positions[i][1] <= currentPos) {
                laterSite = false;
                break;
            }
        }
        if (laterSite == true) {
            return currentPos + 1;
        }
    }

    return strategy_b_others(myPath, participants, id);
}

// Strategy A
// return the next position based on the strategy of Player A
// the purpose of the parameters is similar to next_move_b()
int next_move_a(Path* myPath, Player* player, Participant* participants) {
    int** sites = myPath->sites;
    int id = player->playerId;
    const int* moneys = participants->moneys;
    int currentPos = participants->positions[id][1];
    int nearestBarrier = nearest_barrier(myPath, currentPos);
    int nextMove;

    bool foundMo = false;
    bool foundOthers = false;

    if (sites[currentPos + 1][SITE] == get_type_enum("Mo") &&
            participants->sizes[currentPos + 1] < sites[currentPos + 1][CAPACITY]) {
        nextMove = currentPos + 1;
        foundMo = true;
    }

    for (int i = currentPos + 1; i <= nearestBarrier; i++) {
        if (participants->sizes[i] < sites[i][CAPACITY]) { // check empty space
            if (sites[i][SITE] == get_type_enum("Do")) {
                if ((moneys[id] > 0)) {
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

// Args: 'hapInfo': array of type int saves the read information from HAP
// message. e.g: hapInfo[0] = p, hapInfo[1] = n .... hapInfo[4] = c
// 'arrayLength': legnth of hapInfo = 5;
// 'hapMessage': Message from HAP that does not contain "HAP"
// e.g: "HAP0,2,3,-7,0" -> hapMessage = "0,2,3,-7,0"
// Loop through hapMessage, at position addPoint (which is 2), if
// after the following comma ',' is minus '-', *negativeMoneys = true
// return true iff valid hap and false otherwise
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

// struct 'myPath' is used for getting numberOfSites and pass to handle_move()
//  
// From the complete HAP message from 'buffer'. e.g: "HAP0,2,3,-7,0",
// get 3 first chars and comapare with "HAP", return false if it doesn't match
// After get_hap(), save needed data into struct 'Participant'
// handle the move and return true
bool handle_hap(char* buffer, Path* myPath, Participant* participants) {
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

    if (id >= participants->numberOfPlayers ||
            toPosition >= myPath->numberOfSites ||
            card < 0 || card > numberOfLetters) {
        return false;
    }

    participants->nextTurn = id; 
    participants->moneys[id] += negativeMoneys ? -hapInfo[moneyChange] :
            hapInfo[moneyChange];
    participants->points[id] += hapInfo[addPoint];
    participants->cards[id][card - 1] += 1;

    handle_move(stderr, NULL, myPath, participants, id, toPosition);
    return true;
}

// In this function, display the sites and positions of player
// Continuously get input from stdin and handle with it
// Print the score of the game if get "DONE" from stdin
// Parameters: Struct 'Path', 'Player' and 'Participant' is
// gotten from main() and used to passed to its inner function
// there are 2 playerType: whether 'A' or 'B' to determines which
// player type
// return void;
void handle_input(Path* myPath, Player* player, Participant* participants, char playerType) {
    int defaultBufferSize = 20;
    char* buffer = malloc(sizeof(char) * defaultBufferSize);

    display_game(stderr, myPath, participants);
    while (true) {
        if (read_line(stdin, buffer, &defaultBufferSize)) {
            if (!strcmp(buffer, "YT")) {
                // return a move
                int nextMove = playerType == 'A' ?
                        next_move_a(myPath, player, participants) :
                        next_move_b(myPath, player, participants);
                printf("DO%d\n", nextMove);
                fflush(stdout);
            } else if (!strcmp(buffer, "EARLY")) {
                // Game ended early due to error
                exit(handle_player_errors(EARLY_OVER));
            } else if (!strcmp(buffer, "DONE")) {
                // Game ened normally
                break;
            } else {
                if (!handle_hap(buffer, myPath, participants)) {
                    exit(handle_player_errors(COMMUNICATION_PLAYER));
                }
            }
        } else { // cannot read from stdin
            exit(handle_player_errors(COMMUNICATION_PLAYER));
        }
    }

    calc_scores(stderr, participants);
    free(buffer);
}

// print '^', handle the path. If path file is valid, set up the variables
// and return true. Return false if the path is invalid
bool get_path(Path* myPath, Player* player, Participant* participants) {
    printf("^");
    fflush(stdout);

    handle_path(stdin, myPath, participants->numberOfPlayers);

    if (myPath->valid) {
        set_up(myPath, participants);
    }
    return myPath->valid;
}