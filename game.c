#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "string.h"
#include "game.h"
#include "errors.h"
#include "path.h"
#include "participants.h"
#include "signal.h"

void calc_next_turn(Path* myPath, Participant* pa) {
    int* numberOfSites = &myPath->numberOfSites;
    int* numberOfPlayer = &pa->numberOfPlayers;
    int** positions = pa->positions;
    int* nextTurn = &pa->nextTurn;

    for (int site = 0; site < *numberOfSites; site++) {
        for (int id = 0; id < *numberOfPlayer; id++) {
            if (positions[id][0] == pa->sizes[site] - 1 &&
                    positions[id][1] == site) {
                *nextTurn = id;
                return;
            }
        }
    }
}

void set_up(Path* myPath, Participant* pa) {
    const int* numberOfPlayers = &pa->numberOfPlayers;
    int** positions = malloc(sizeof(int*) * (* numberOfPlayers));
    int** cards = malloc(sizeof(int*) * (* numberOfPlayers));
    int* moneys = malloc(sizeof(int) * (* numberOfPlayers));
    int* points = malloc(sizeof(int) * (* numberOfPlayers));
    int* sizes = malloc(sizeof(int) * (myPath->numberOfSites));
    int* v1 = malloc(sizeof(int) * (* numberOfPlayers));
    int* v2 = malloc(sizeof(int) * (* numberOfPlayers));
    int* pointChange = malloc(sizeof(int) * (* numberOfPlayers));
    int* moneyChange = malloc(sizeof(int) * (* numberOfPlayers));
    int* nextMove = malloc(sizeof(int) * (* numberOfPlayers));

    int largestId = *numberOfPlayers - 1;
    for (int id = 0; id < *numberOfPlayers; id++) {
        moneys[id] = DEFAULT_MONEY;
        points[id] = DEFAULT_VALUE;
        v1[id] = DEFAULT_VALUE;
        v2[id] = DEFAULT_VALUE;
        pointChange[id] = DEFAULT_VALUE;
        moneyChange[id] = DEFAULT_VALUE;
        nextMove[id] = DEFAULT_VALUE;
        cards[id] = malloc(sizeof(int) * NUM_A_TO_E);
        for (int j = 0; j < NUM_A_TO_E; j++) {
            // cards[id][CARD] = 'A' + id;
            cards[id][j] = DEFAULT_VALUE;
        }
        positions[id] = malloc(sizeof(int) * 2);
        positions[id][0] = largestId--;
        positions[id][1] = DEFAULT_VALUE;
    }

    for (int site = 0; site < myPath->numberOfSites; site++) {
        if (site == 0) {
            sizes[site] = *numberOfPlayers;
        } else {
            sizes[site] = 0;
        }
    }

    myPath->sites[0][CAPACITY] = 0;
    pa->positions = positions;
    pa->moneys = moneys;
    pa->points = points;
    pa->v1 = v1;
    pa->v2 = v2;
    pa->pointChange = pointChange;
    pa->moneyChange = moneyChange;
    pa->cards = cards;
    pa->nextMove = nextMove;
    pa->sizes = sizes;
    // remember to free !!!!!!!!!
}

int is_valid_move(Path* myPath, Participant* pa,
        const int playerId, const int toPosition) {
    int* currentPosition = &pa->positions[playerId][1];
    int closestBarrier = nearest_barrier(myPath, currentPosition);
    int nextSize = pa->sizes[toPosition];
    int nextCap = myPath->sites[toPosition][CAPACITY];

    return (toPosition <= closestBarrier) && (nextSize < nextCap) &&
            (toPosition > *currentPosition);
}

void handle_move(FILE* file, Path* myPath, Participant* pa, int playerId,
        const int toPosition) {
    //position:
    int** sites = myPath->sites;
    int** positions = pa->positions;
    int* currentPosition = &positions[playerId][1];
    int* currentRow = &positions[playerId][0];
    int* playerMoney = &(pa->moneys)[playerId];
    int nextSite = sites[toPosition][SITE];

    // printf("To: %d\n", toPosition);
    if (!is_valid_move(myPath, pa, playerId, toPosition)) {
        return;
    }

    if (file == stdout) { // dealer
        if (nextSite == MONEY) {
            *playerMoney += 3;
        } else if (nextSite == MONEY_2_POINT) {
            pa->pointChange[playerId] = (int)floor(*playerMoney / 2);
            pa->moneyChange[playerId] = -(*playerMoney);
            pa->points[playerId] += pa->pointChange[playerId];
            *playerMoney = 0;
        }
    }

    switch (nextSite) {
        case V1:
            (pa->v1)[playerId] += 1;
            break;
        case V2:
            (pa->v2)[playerId] += 1;
            break;
        case DRAW_NEXT_CARD:
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
        char* currentSite = get_type_char(&sites[i][SITE]);
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

    int largestSize = find_max(pa->sizes, *numberOfSites);
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
            (pa->nextTurn), (pa->moneys)[pa->nextTurn], (pa->v1)[pa->nextTurn],
            (pa->v2)[pa->nextTurn], (pa->points)[pa->nextTurn]);
    fflush(file);
    for (int i = 0; i < NUM_A_TO_E; i++) {
        fprintf(file, "%c=%d", 'A' + i, (pa->cards)[pa->nextTurn][0]);
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
    // display_dealer_output(myPath, pa);
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
    int* points = pa->points;
    int* numberOfPlayers = &pa->numberOfPlayers;
    fprintf(file, "Scores: ");
    for (int id = 0; id < *numberOfPlayers; id++) {
        pa->points[id] += (pa->v1[id] + pa->v2[id]);
        fprintf(file, "%d", points[id]);
        if (id != *numberOfPlayers - 1) {
            fprintf(file, ",");
        } else {
            fprintf(file, "\n");
        }
    }
}

bool receivedSighub = false;
void notice(int s) {
    receivedSighub = true;
}

void send_last_message(const int* numberOfPlayers, 
        FILE** writeFile, FILE** readFile, int** pipesWrite, int** pipesRead,
        bool early) {
    
    for (int id = 0; id < *numberOfPlayers; id ++) {
        if (early) {
            fprintf(writeFile[id], "EARLY\n");
            fflush(writeFile[id]);        
        } else {
            fprintf(writeFile[id], "DONE\n");
            fflush(writeFile[id]);
        }

        // Close pipes and files
        fclose(writeFile[id]);
        fclose(readFile[id]);
        close(pipesWrite[id][WRITE_END]);
        close(pipesWrite[id][READ_END]);
        close(pipesRead[id][WRITE_END]);
        close(pipesRead[id][READ_END]);
    }
}

void initial_game(Deck* myDeck, Path* myPath, Participant* pa, char** argv) {
    set_up(myPath, pa);
    display_game(stdout, myPath, pa);

    // SIGACTION
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = notice;   
    action.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &action, 0);
    
    const int* numberOfPlayers = &pa->numberOfPlayers; // 2 (int)
    char* playersCountStr = number_to_string(*numberOfPlayers); // "2" (char*)
    int playerPosition = 3; // first position of executable program is 3
    FILE** writeFile = malloc(sizeof(FILE*) * (*numberOfPlayers));
    FILE** readFile = malloc(sizeof(FILE*) * (*numberOfPlayers));
    int** pipesRead = malloc(sizeof(int*) * (*numberOfPlayers));
    int** pipesWrite = malloc(sizeof(int*) * (*numberOfPlayers));

    for (int id = 0; id < *numberOfPlayers; id++) {
        char* currentPlayer = argv[playerPosition++]; // "./2310A"
        //create two pipe
        pipesWrite[id] = malloc(sizeof(int) * 2);
        pipesRead[id] = malloc(sizeof(int) * 2);

        if (pipe(pipesWrite[id]) == -1 || pipe(pipesRead[id]) == -1) {
            exit(handle_error_message(STARTING_PROCESS));
        }

        pid_t process = fork();

        if (process == -1) {
            exit(handle_error_message(STARTING_PROCESS));
        } else if (process == 0) { // Child
            close(pipesWrite[id][WRITE_END]);
            close(pipesRead[id][READ_END]);

            dup2(pipesRead[id][WRITE_END], 1);
            close(pipesRead[id][WRITE_END]);


            dup2(pipesWrite[id][READ_END], 0);
            close(pipesWrite[id][READ_END]);

            int nullDescriptor = open("/dev/null", O_WRONLY);
            dup2(nullDescriptor, 2);
            close(nullDescriptor);

            char* playerIdStr = number_to_string(id);
            if (execlp(currentPlayer, currentPlayer,
                    playersCountStr, playerIdStr, NULL) == -1) {
                free(playerIdStr);
                exit(handle_error_message(STARTING_PROCESS));
            }

            free(playerIdStr);
        } else { // Parent
            close(pipesWrite[id][READ_END]);
            close(pipesRead[id][WRITE_END]);

            writeFile[id] = fdopen(pipesWrite[id][WRITE_END], "w");
            readFile[id] = fdopen(pipesRead[id][READ_END], "r");
            
            // Read ^
            fflush(stdout);
            char msg = (char)(fgetc(readFile[id]));
            fflush(stdout);

            if (msg == '^') {
                // send path
                fprintf(writeFile[id], "%s", myPath->rawFile);
                fflush(writeFile[id]);
            } else {
                send_last_message(numberOfPlayers, writeFile, readFile,
                        pipesWrite, pipesRead, true);
                exit(handle_error_message(COMMUNICATION));
            }
        }
    }

    while (!is_end_game(myPath, pa)) {
        /// !!!!!!!!!!!!!!!!
        if (receivedSighub) {
            send_last_message(numberOfPlayers, writeFile, readFile,
            pipesWrite, pipesRead, receivedSighub);
            while ((wait(NULL)) >- 1);
        }

        calc_next_turn(myPath, pa);

        // YT
        fprintf(writeFile[pa->nextTurn], "YT\n");
        fflush(writeFile[pa->nextTurn]);

        // Get next move
        char firstLetter;
        char secondLetter;
        char newLine;
        fscanf(readFile[pa->nextTurn], "%c%c%d%c", &firstLetter, &secondLetter,
            &(pa->nextMove)[pa->nextTurn], &newLine);
        // check whether message follows formar "DO" + site + '\n or not
        if (firstLetter != 'D' || secondLetter != 'O' ||
                pa->nextMove[pa->nextTurn] > myPath->numberOfSites ||
                newLine != '\n') { // Comms error
            send_last_message(numberOfPlayers, writeFile, readFile,
                    pipesWrite, pipesRead, true);
        }
        // Handle Move
        handle_move(stdout, myPath, pa,
                pa->nextTurn, (pa->nextMove)[pa->nextTurn]);

        for (int id = 0; id < *numberOfPlayers; id++) {
            // Send HAP
            fprintf(writeFile[id], "HAP%d,%d,%d,%d,%d\n", (pa->nextTurn),
            (pa->nextMove)[pa->nextTurn], (pa->pointChange)[pa->nextTurn],
            (pa->moneyChange)[pa->nextTurn], (pa->cards)[pa->nextTurn][0]); // change 0
            fflush(writeFile[id]);     
        }

    }

    send_last_message(numberOfPlayers, writeFile, readFile, pipesWrite,
            pipesRead, receivedSighub);

    calc_scores(stdout, pa);

    free(playersCountStr);
}