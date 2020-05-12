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

bool endOfChild = false;

void sigchild_handler(int s) {
    endOfChild = true;
}

void calc_next_turn(Path* myPath, Participant* pa) {
    int numberOfSites = myPath->numberOfSites;
    int numberOfPlayer = pa->numberOfPlayers;
    int** positions = pa->positions;
    int* nextTurn = &pa->nextTurn;

    for (int site = 0; site < numberOfSites; site++) {
        for (int id = 0; id < numberOfPlayer; id++) {
            if (positions[id][0] == pa->sizes[site] - 1 &&
                    positions[id][1] == site) {
                *nextTurn = id;
                return;
            }
        }
    }
}

void set_up(Path* myPath, Participant* pa) {
    const int defaultMoney = 7;
    int numberOfPlayers = pa->numberOfPlayers;
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

int is_valid_move(Path* myPath, Participant* pa,
        const int playerId, const int toPosition) {
    int currentPosition = pa->positions[playerId][1];
    int closestBarrier = nearest_barrier(myPath, currentPosition);
    int nextSize = pa->sizes[toPosition];
    int nextCap = myPath->sites[toPosition][CAPACITY];

    return (toPosition <= closestBarrier) && (nextSize < nextCap) &&
            (toPosition > currentPosition);
}

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

    display_dealer_output(file, myPath, pa);
    display_game(file, myPath, pa);
}

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

void display_player_position(FILE* file, Path* myPath, Participant* pa) {
    int* numberOfSites = &myPath->numberOfSites;
    int* numberOfPlayers = &pa->numberOfPlayers;
    int** positions = pa->positions;

    int largestSize = find_max(pa->sizes, *numberOfSites, NULL);
    for (int id = 0; id < largestSize; id++) {
        for (int site = 0; site < *numberOfSites; site++) {
            bool found = false;
            for (int i = 0; i < *numberOfPlayers; i++) {
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
                    if (site == *numberOfSites - 1) {
                        fprintf(file, "\n");
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                continue;
            }
            fprintf(file, "   ");
            fflush(stdout);
            if (site == *numberOfSites - 1) {
                fprintf(file, "\n");
            }
        }
    }
}

void display_dealer_output(FILE* file, Path* myPath, Participant* pa) {
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

void display_game(FILE* file, Path* myPath, Participant* pa) {
    display_sites(file, myPath);
    display_player_position(file, myPath, pa);
}

bool is_end_game(Path* myPath, Participant* pa) {
    int lastBarrier = myPath->numberOfSites - 1;
    for (int id = 0; id < pa->numberOfPlayers; id++) {
        if (pa->positions[id][1] != lastBarrier) {
            return false;
        }
    }
    return true;
}

void calc_scores(FILE* file, Participant* pa) {
    int** cards = pa->cards;
    int* points = pa->points;
    int numberOfPlayers = pa->numberOfPlayers;
    fprintf(file, "Scores: ");
    for (int id = 0; id < numberOfPlayers; id++) {
        // Add V1,V2 scores
        pa->points[id] += (pa->siteV1[id] + pa->siteV2[id]);
        // Add scores by collecting cards
        int maxPointsOfASet = 10;
        bubble_sort(cards[id], NUM_A_TO_E);
        for (int i = 0; i < NUM_A_TO_E; i++) {
            int pointsChange;
            if (i == 0) {
                pointsChange = cards[id][i] * maxPointsOfASet;
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

void close_pipes_and_files(int id, int** pipesWrite, int** pipesRead,
        FILE** writeFile, FILE** readFile) {
    fclose(writeFile[id]);
    fclose(readFile[id]);
    close(pipesWrite[id][WRITE_END]);
    close(pipesWrite[id][READ_END]);
    close(pipesRead[id][WRITE_END]);
    close(pipesRead[id][READ_END]);
}

void send_last_message(pid_t* childIds, int numberOfPlayers, 
        FILE** writeFile, FILE** readFile, int** pipesWrite, int** pipesRead,
        bool early) {
    const int emptyValue = -1;
    pid_t pid;
    while (pid = waitpid(-1, 0, WNOHANG), pid > 0) {
        for (int i = 0; i < numberOfPlayers; i++) {
            if (childIds[i] == pid) {
                childIds[i] = emptyValue;
            }
        }
    }
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

    // reap again in case of the players cannot get messsage from dealers
    for (int i = 0; i < numberOfPlayers; i++) {
        if (childIds[i] > 0) {
            kill(childIds[i], SIGKILL);
            waitpid(childIds[i], 0, WNOHANG);
        }
    }

    if (early) {
        exit(handle_error_message(COMMUNICATION));
    }
}

void handle_child(int id, char* currentPlayer, char* playersCountString,
        FILE** writeFile, FILE** readFile,
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

void initial_game(int numberOfPlayers, FILE** writeFile, FILE** readFile,
        int** pipesWrite, int** pipesRead, pid_t* childIds, char* rawPath,
        char** argv) {
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
            handle_child(id, currentPlayer, playersCountString, writeFile,
                    readFile, pipesWrite, pipesRead);
        } else { // Parent
            handle_parent(id, rawPath, writeFile, readFile, pipesWrite,
                    pipesRead);
        }
    }

    free(playersCountString);
}

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
        // check whether message follows formar "DO" + site + '\n or not
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