#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "game.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "deck.h"

// global variable to change when receive SIGCHILD
bool endOfChild = false;

// function handler when receive SIGCHILD
// receive an int in parameter and return void;
void sigchild_handler(int s) {
    endOfChild = true;
}

// With this function, dealer can determines which player should go next
// based on their current position (get from struct 'Participant')
// and the number of sites (get from struct 'Path')
// Then save the next turn (e.g: 0) into pa->nextTurn
// return void;
void calc_next_turn(Path* myPath, Participant* pa) {
    int numberOfSites = myPath->numberOfSites;
    int numberOfPlayer = pa->numberOfPlayers;
    int** positions = pa->positions;
    int* nextTurn = &pa->nextTurn;

    // left to right
    for (int site = 0; site < numberOfSites; site++) {
        // bottom to top
        for (int id = 0; id < numberOfPlayer; id++) {
            // if the player is at lowest position
            if (positions[id][0] == pa->sizes[site] - 1 &&
                    // site of that player is the current site
                    positions[id][1] == site) {
                *nextTurn = id;
                return;
            }
        }
    }
}

// Malloc most of variables of struct 'Participant' with needed sizes and set up
// their default values. For example, default positions for all players
// at the beginning of the game is at the first site with id from high to low;
// and default points and money are 0 and 7 respectively, etc.
// For some variables, it needs 'numberOfSites' of struct 'Path'
// return void;
void set_up(Path* myPath, Participant* pa) {
    const int defaultMoney = 7;
    int numberOfPlayers = pa->numberOfPlayers;
    // These variables are explained in Participants.h
    int** positions = malloc(sizeof(int*) * numberOfPlayers);
    int** cards = malloc(sizeof(int*) * numberOfPlayers);
    int* moneys = malloc(sizeof(int) * numberOfPlayers);
    int* points = malloc(sizeof(int) * numberOfPlayers);
    int* sizes = malloc(sizeof(int) * (myPath->numberOfSites));
    int* siteV1 = malloc(sizeof(int) * numberOfPlayers);
    int* siteV2 = malloc(sizeof(int) * numberOfPlayers);
    int* pointChange = malloc(sizeof(int) * numberOfPlayers);
    int* moneyChange = malloc(sizeof(int) * numberOfPlayers);
    int* nextMove = malloc(sizeof(int) * numberOfPlayers);

    int largestId = numberOfPlayers - 1;
    for (int id = 0; id < numberOfPlayers; id++) {
        moneys[id] = defaultMoney;
        points[id] = DEFAULT_VALUE;
        siteV1[id] = siteV2[id] = DEFAULT_VALUE;
        pointChange[id] = moneyChange[id] = DEFAULT_VALUE;
        nextMove[id] = DEFAULT_VALUE;
        cards[id] = malloc(sizeof(int) * NUM_A_TO_E);
        for (int j = 0; j < NUM_A_TO_E; j++) {
            cards[id][j] = DEFAULT_VALUE;
        }
        positions[id] = malloc(sizeof(int) * 2);
        positions[id][0] = largestId--;
        positions[id][1] = DEFAULT_VALUE;
    }

    for (int site = 0; site < myPath->numberOfSites; site++) {
        if (site == 0) {
            sizes[site] = numberOfPlayers;
        } else {
            sizes[site] = 0;
        }
    }

    pa->positions = positions;
    pa->moneys = moneys;
    pa->points = points;
    pa->siteV1 = siteV1;
    pa->siteV2 = siteV2;
    pa->pointChange = pointChange;
    pa->moneyChange = moneyChange;
    pa->cards = cards;
    pa->nextMove = nextMove;
    pa->sizes = sizes;
}

// return true iff given playerId want to move to toPosition is
// a valid destination
bool is_valid_move(Path* myPath, Participant* pa,
        const int playerId, int toPosition) {
    int currentPosition = pa->positions[playerId][1];
    int closestBarrier = nearest_barrier(myPath, currentPosition);
    int nextSize = pa->sizes[toPosition];
    int nextCap = myPath->sites[toPosition][CAPACITY];

    return (toPosition <= closestBarrier) && (nextSize < nextCap) &&
            (toPosition > currentPosition);
}

// This function is used for both dealer and player, based on given File* file
// to handle with playerId move to which position
// If dealer: file == stdout, dealer will check the type of the
// next site (toPosition) and handle with it.
// (e.g: +3 money if next site is Mo)
// If player: file == stderr, are allowed to update their V1 or V2 status
// For dealer and players:
// Increase and decrease the capacity of their old position and new position
// respectively and display to the screen the output from dealer, sites and
// new position after the move.
// Others parameters:
// - Get next site from struct 'Path'
// - If next site is 'Ri', get next card from struct 'Deck'
// - Get positions of players, their current points, moneys
//   from struct 'Participant'
// Return void;
void handle_move(FILE* file, Deck* myDeck, Path* myPath, Participant* pa,
        int playerId, const int toPosition) {
    int** positions = pa->positions;
    int* currentPosition = &positions[playerId][1];
    int* currentRow = &positions[playerId][0];
    int* playerMoney = &(pa->moneys)[playerId];
    int nextSite = myPath->sites[toPosition][SITE];
    pa->nextCard = 0;
    pa->pointChange[playerId] = 0;
    pa->moneyChange[playerId] = 0;

    if (file == stdout) { // dealer
        if (nextSite == MONEY) {
            *playerMoney += 3;
            pa->moneyChange[playerId] = 3;
        } else if (nextSite == MONEY_2_POINT) {
            pa->pointChange[playerId] = (int)floor(*playerMoney / 2);
            pa->moneyChange[playerId] = -(*playerMoney);
            pa->points[playerId] += pa->pointChange[playerId];
            *playerMoney = 0;
        } else if (nextSite == DRAW_NEXT_CARD) {
            char card = get_next_card(myDeck);
            pa->nextCard = get_card_enum(card);
            pa->cards[playerId][pa->nextCard - 1]++; 
        }
    }

    switch (nextSite) {
        case SITE_V1:
            (pa->siteV1)[playerId] += 1;
            break;
        case SITE_V2:
            (pa->siteV2)[playerId] += 1;
            break;
        default:
            break;
    }

    pa->sizes[*currentPosition]--;
    *currentPosition = toPosition;
    *currentRow = pa->sizes[toPosition];
    pa->sizes[toPosition]++;

    display_dealer_output(file, pa);
    display_game(file, myPath, pa);
}

// Used for both dealer or player: FILE* file is stdout or stderr
// Display to the screen the sites (e.g: :: Mo Do Ri ::)
// based on the sites and numberOfSites taken from struct 'Path'
void display_sites(FILE* file, Path* myPath) {
    int* numberOfSites = &myPath->numberOfSites;
    int** sites = myPath->sites;
    for (int i = 0; i < *numberOfSites; i++) {
        char* currentSite = get_type_char(sites[i][SITE]);
        fprintf(file, "%s ", currentSite);
        fflush(file);
    }
    fprintf(file, "\n");
    fflush(file);
}

// Used for both dealer or player: FILE* file is stdout or stderr
// Based on the positions of players stored in struct 'Participant',
// and numberOfSites in struct 'Path',
// then Display to the screen with right orders.
// return void;
void display_player_position(FILE* file, Path* myPath, Participant* pa) {
    int numberOfSites = myPath->numberOfSites;
    int numberOfPlayers = pa->numberOfPlayers;
    int** positions = pa->positions;

    int largestSize = find_max(pa->sizes, numberOfSites, NULL);
    // Loop for the rows from 0 to largestSize to avoid to print the
    // row which does not have any player in it
    for (int id = 0; id < largestSize; id++) {
        for (int site = 0; site < numberOfSites; site++) {
            bool found = false;
            for (int i = 0; i < numberOfPlayers; i++) {
                if (positions[i][0] == id && positions[i][1] == site) {
                    fprintf(file, "%d", i);
                    switch (count_number_digit(i)) {
                        case 1:
                            fprintf(file, "  "); // 0 <= i <= 9
                            break;
                        case 2:
                            fprintf(file, " "); // 10 <= i <= 99
                            break;
                        default:
                            break; // i >= 100
                    }
                    fflush(stdout);
                    if (site == numberOfSites - 1) {
                        fprintf(file, "\n");
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                continue;
            }
            // When does not found player at a position, print 3 spaces
            // Otherwise, print playerId + spaces (at most 2)
            fprintf(file, "   ");
            fflush(stdout);
            // End of line
            if (site == numberOfSites - 1) {
                fprintf(file, "\n");
            }
        }
    }
}

// Display output from dealer. e.g: Player 0 Money=7 V1=0 .....
// based on the information get from struct 'Participant'
// return void;
void display_dealer_output(FILE* file, Participant* pa) {
    fprintf(file, "Player %d Money=%d V1=%d V2=%d Points=%d ",
            (pa->nextTurn), (pa->moneys)[pa->nextTurn],
            (pa->siteV1)[pa->nextTurn],
            (pa->siteV2)[pa->nextTurn], (pa->points)[pa->nextTurn]);
    fflush(file);
    for (int i = 0; i < NUM_A_TO_E; i++) {
        fprintf(file, "%c=%d", 'A' + i, (pa->cards)[pa->nextTurn][i]);
        fflush(file);
        if (i != (NUM_A_TO_E - 1)) {
            fprintf(file, " ");
            fflush(file);
        }
    }
    fprintf(file, "\n");
    fflush(file);
}

// display_game() includes display_sites() and display_player_position()
// the parameters of this function already explained at two
// nearest functions above
// return void;
void display_game(FILE* file, Path* myPath, Participant* pa) {
    display_sites(file, myPath);
    display_player_position(file, myPath, pa);
}

// Return true iff all players are at the last barrier and false otherwise
// based on the position of last barrier from struct 'Path' and
// the number of players from struct 'Participant'
bool is_end_game(Path* myPath, Participant* pa) {
    int lastBarrier = myPath->numberOfSites - 1;
    for (int id = 0; id < pa->numberOfPlayers; id++) {
        if (pa->positions[id][1] != lastBarrier) {
            return false;
        }
    }
    return true;
}

// Calculate scores and then display the message to the screen
// based on the data get from struct 'Participant' likes
// cards, points or number of V1 site each player went by
// This function both used for players ('file' == stderr) and
// dealer ('file' == stdout)
// e.g: "Scores: 10,12\n"
// return void;
void calc_scores(FILE* file, Participant* pa) {
    int** cards = pa->cards;
    int* points = pa->points;
    int numberOfPlayers = pa->numberOfPlayers;
    fprintf(file, "Scores: ");
    for (int id = 0; id < numberOfPlayers; id++) {
        // Add V1,V2 scores
        pa->points[id] += (pa->siteV1[id] + pa->siteV2[id]);
        // Add scores by collecting cards
        int maxPointsOfASet = 10; // Set of 5 cards
        bubble_sort(cards[id], NUM_A_TO_E);
        for (int i = 0; i < NUM_A_TO_E; i++) {
            int pointsChange;
            if (i == 0) {
                // After the arrays is sorted, get the min * 10
                pointsChange = cards[id][i] * maxPointsOfASet;
                // maxPoint then equals to 7
                maxPointsOfASet -= 3;
            } else {
                pointsChange = (cards[id][i] - cards[id][i - 1]) *
                        maxPointsOfASet;
                maxPointsOfASet -= 2;
            }
            pa->points[id] += pointsChange;
        }
        // Print Scores:
        fprintf(file, "%d", points[id]);
        if (id != numberOfPlayers - 1) {
            fprintf(file, ",");
        } else {
            fprintf(file, "\n");
        }
    }
}

// Close the files and pipes that are left based on the 'id' of the
// players.
// 'pipesWrite', 'pipesRead', 'writeFile', 'readFile'
// all have sizes of numberOfPlayers.
// In each int* pipesWrite, there are 2 ends of dealer: WRITE_END
// and 'READ_END'. Similar for pipesRead
// FILE* writeFile is the file pointer with respect to the file descriptor
// write to pipesWrite of each player
// Similar for FILE* readFile
// Return void;
void close_pipes_and_files(int id, int** pipesWrite, int** pipesRead,
        FILE** writeFile, FILE** readFile) {
    fclose(writeFile[id]);
    fclose(readFile[id]);
    close(pipesWrite[id][WRITE_END]);
    close(pipesWrite[id][READ_END]);
    close(pipesRead[id][WRITE_END]);
    close(pipesRead[id][READ_END]);
}

// Based on 'numberOfPlayers' and 'childIds',
// Send "DONE" to all of the players ('early' == false) 
// or "EARLY" to players that are not terminated by signal' ('early' == true)
// After send to the pipes, close files and pipes (get from parameters)
// of each players kill and reap the "alive" players in case of 
// the players are not exit. If early == true, exit dealer with Comms error
// return void;
void send_last_message(pid_t* childIds, int numberOfPlayers, 
        FILE** writeFile, FILE** readFile, int** pipesWrite, int** pipesRead,
        bool early) {
    const int emptyValue = -1;
    pid_t pid;

    // Wait NOHANG for the child (In case the child is termintaed by signal)
    while (pid = waitpid(-1, 0, WNOHANG), pid > 0) {
        for (int i = 0; i < numberOfPlayers; i++) {
            if (childIds[i] == pid) {
                // Change the pid of terminated child into -1 in the array
                childIds[i] = emptyValue;
            }
        }
    }
    // Only send message to the "alive" ones to prevent SIGPIPE
    for (int id = 0; id < numberOfPlayers && childIds[id] != -1; id++) {
        if (early) {
            fprintf(writeFile[id], "EARLY\n");
            fflush(writeFile[id]);
        } else {
            fprintf(writeFile[id], "DONE\n");
            fflush(writeFile[id]);
        }
        // Close pipes and files
        close_pipes_and_files(id, pipesWrite, pipesRead, writeFile, readFile);
    }

    if (early) {
        exit(handle_error_message(COMMUNICATION));
    }
}

// After fork(), each player will close WRITE_END of its 'pipesWrite' and
// READ_END of its 'pipesRead' based on its id, then dup2() and execlp
// with given 'currentPlayer' (e.g: "./2310A") and 'playersCountString'
// (e.g: "4") and its id as a string (e.g: "0")
// return void;
void handle_child(int id, char* currentPlayer, char* playersCountString,
        int** pipesWrite, int** pipesRead) {
    close(pipesWrite[id][WRITE_END]);
    close(pipesRead[id][READ_END]);

    dup2(pipesRead[id][WRITE_END], 1);
    close(pipesRead[id][WRITE_END]);

    dup2(pipesWrite[id][READ_END], 0);
    close(pipesWrite[id][READ_END]);

    int nullDescriptor = open("/dev/null", O_WRONLY);
    dup2(nullDescriptor, 2);
    close(nullDescriptor);

    char* playerIdString = number_to_string(id);
    if (execlp(currentPlayer, currentPlayer,
            playersCountString, playerIdString, NULL) == -1) {
        free(playerIdString);
        exit(handle_error_message(STARTING_PROCESS));
    }

    free(playerIdString);
}

// The dealer (parent) will run this function, close READ_END and WRITE_END of
// each pipes, fdopen() and read caret '^' from player and
// send the path char* rawPath to the pipes with given id in parameter
// if it doesnt get the '^' character, exit with starting process error code
// return void;
void handle_parent(int id, char* rawPath, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead) {
    close(pipesWrite[id][READ_END]);
    close(pipesRead[id][WRITE_END]);
    writeFile[id] = fdopen(pipesWrite[id][WRITE_END], "w");
    readFile[id] = fdopen(pipesRead[id][READ_END], "r");

    // Read '^'
    char msg = (char)(fgetc(readFile[id]));
    fflush(stdout);

    if (msg == '^') {
        // send path
        fprintf(writeFile[id], "%s", rawPath);
        fflush(writeFile[id]);
    } else {
        exit(handle_error_message(STARTING_PROCESS));
    }
}

// For each player in numberOfPlayers, malloc pipesRead and pipesWrite with
// size of 2 int, fork() and save id of each childs in pid_t*
// run handle_child() and handle_parent()
// From argv, get "./2310A" for example and pass down to handle_child()
// return void;
void initial_game(int numberOfPlayers, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead, pid_t* childIds, char* rawPath,
        char** argv) {
    // e.g: ./2310dealer d1.deck p1.path ./2310A ./2310A
    int playerPosition = 3; // first position of executable program is 3
    char* playersCountString = number_to_string(numberOfPlayers); // "2"
    for (int id = 0; id < numberOfPlayers; id++) {
        char* currentPlayer = argv[playerPosition++]; // "./2310A"
        //create two pipe
        pipesWrite[id] = malloc(sizeof(int) * 2);
        pipesRead[id] = malloc(sizeof(int) * 2);

        if (pipe(pipesWrite[id]) == -1 || pipe(pipesRead[id]) == -1) {
            exit(handle_error_message(STARTING_PROCESS));
        }

        childIds[id] = fork();

        if (childIds[id] == -1) {
            exit(handle_error_message(STARTING_PROCESS));
        } else if (childIds[id] == 0) { // Child
            handle_child(id, currentPlayer, playersCountString,
                    pipesWrite, pipesRead);
        } else { // Parent
            handle_parent(id, rawPath, writeFile, readFile, pipesWrite,
                    pipesRead);
        }
    }

    free(playersCountString);
}

// After fork() and send Path to all parents
// In this function, run display_game(), while the game is not end, determines
// which player should go next. To avoid SIGPIPE, check whether that player
// is alive or not before sending "YT", read "DO", exit if comms error with
// that player require invalid move. Otherwisem handle_move() and send "HAP"
// to all players.
// This function needs to pass all of its parameters that it gets from
// function run_game() (mentioned below) to its inner functions
// This function also needs the numberOfPlayers from struct 'Participant'
// return void;
void communicate(Deck* myDeck, Path* myPath, Participant* pa, pid_t* childIds,
        FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead) {
    int numberOfPlayers = pa->numberOfPlayers;
    display_game(stdout, myPath, pa);
    while (!is_end_game(myPath, pa)) {
        char firstLetter;
        char secondLetter;
        char newLine;
        calc_next_turn(myPath, pa);
        // Send YT
        if (endOfChild) { // check end of child to avoid SIGPIPE
            send_last_message(childIds, numberOfPlayers, writeFile, readFile,
                    pipesWrite, pipesRead, true);
        }
        fprintf(writeFile[pa->nextTurn], "YT\n");
        fflush(writeFile[pa->nextTurn]);

        // Read DO
        int read = fscanf(readFile[pa->nextTurn], "%c%c%d%c", &firstLetter,
                &secondLetter, &(pa->nextMove)[pa->nextTurn], &newLine);
        // check whether message follows format "DO" + site + '\n' or not
        if (read != 4 || firstLetter != 'D' || secondLetter != 'O' ||
                newLine != '\n' || !is_valid_move(myPath, pa, pa->nextTurn,
                pa->nextMove[pa->nextTurn])) { // Comms error
            send_last_message(childIds, numberOfPlayers, writeFile, readFile,
                    pipesWrite, pipesRead, true);
        }

        handle_move(stdout, myDeck, myPath, pa,
                pa->nextTurn, (pa->nextMove)[pa->nextTurn]);

        // Send HAP
        if (endOfChild) { // check end of child to avoid SIGPIPE
            send_last_message(childIds, numberOfPlayers, writeFile, readFile,
                    pipesWrite, pipesRead, true);
        }
        for (int id = 0; id < numberOfPlayers; id++) {
            fprintf(writeFile[id], "HAP%d,%d,%d,%d,%d\n",
                    (pa->nextTurn), (pa->nextMove)[pa->nextTurn],
                    (pa->pointChange)[pa->nextTurn],
                    (pa->moneyChange)[pa->nextTurn], pa->nextCard);
            fflush(writeFile[id]);
        }
    }
}

// set up sigaction of SIGCHILD, set up variables and runs the game
// When the game is over, send "DONE" to all players
// All of the parameters of this function is gotten from the main() function
// in order to pass to its inner function. The purpose of those parameters are
// all mentioned previously
// return void;
void run_game(Deck* myDeck, Path* myPath, Participant* pa, char** argv) {
    struct sigaction sigchildAction;
    sigchildAction.sa_handler = sigchild_handler;
    sigchildAction.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sigchildAction, 0);   

    // Set up default variables
    set_up(myPath, pa);
    
    int numberOfPlayers = pa->numberOfPlayers; // 2 (int)
    char* rawPath = myPath->rawPath;
    FILE** writeFile = malloc(sizeof(FILE*) * numberOfPlayers);
    FILE** readFile = malloc(sizeof(FILE*) * numberOfPlayers);
    int** pipesRead = malloc(sizeof(int*) * numberOfPlayers);
    int** pipesWrite = malloc(sizeof(int*) * numberOfPlayers);
    pid_t childIds[numberOfPlayers];

    initial_game(numberOfPlayers, writeFile, readFile,
            pipesWrite, pipesRead, childIds, rawPath, argv);

    communicate(myDeck, myPath, pa, childIds, writeFile, readFile,
            pipesWrite, pipesRead);

    // End game, send "DONE"
    send_last_message(childIds, numberOfPlayers, writeFile, readFile,
            pipesWrite, pipesRead, false);

    calc_scores(stdout, pa);
}