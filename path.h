#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <stdio.h>
#include "participants.h"

// In variable int** sites of struct Path
#define SITE 0      // sites[site][SITE] stores the site (as enum)
#define CAPACITY 1  // sites[site][CAPACITY] stores the capacity
                    // of the correspoinding site

// In each pipe created
#define READ_END 0  // index 0 is read end
#define WRITE_END 1 // index 1 is write end

#define DEFAULT_VALUE 0 // deafault value of some variables
                        // e.g: at the beginning of the game, each player
                        // has 0 point, V1=0, V2=0, etc

// Defines each site of as a number
typedef enum Sites {
    MONEY = 0,          // Mo
    SITE_V1 = 1,        // V1
    SITE_V2 = 2,        // V2
    MONEY_2_POINT = 3,  // Do
    DRAW_NEXT_CARD = 4, // Ri
    BARRIER = 5,        // ::
    ERROR_TYPE = 6
} SiteType;

// Defines needed variables for a path
typedef struct { // e.g: 8;::-Mo1Ri1Do3V14V25Do2::-
    int numberOfSites;       // 8
    char* rawPath;           // "8;::-Mo1Ri1Do3V14V25Do2::-\n"
    int* availableCapacity;  // [1,1,3,4,5,2]
    int** sites;             // sites[1][SITE] = 0 (enum of "Mo")
                             // sites[1][CAPACITY] = 1
    bool valid;              // Valid path? true
} Path;

SiteType get_type_enum(const char* site);

char* get_type_char(SiteType site);

SiteType get_site(int position, const char* tempSites);

void get_sites(Path* myPath, const char* tempSites, int numberOfPlayers);

void handle_path(FILE* pathFile, Path* myPath, int numberOfPlayers);

bool is_valid_path(char* path, Path* myPath, int numberOfPlayers);

int nearest_barrier(Path* myPath, int currentPosition);

#endif