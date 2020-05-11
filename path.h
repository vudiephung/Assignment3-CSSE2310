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

// return corresponding SiteType of the parameter char* site
// e.g: return MONEY (0) iff site == "Mo"
SiteType get_type_enum(const char* site);

// With given SiteType, return corresponding char*
// e.g: get_type_char(5) == "::"
char* get_type_char(SiteType site);

// Parameters: char* tempSite is a path that does not include number and ';'
// e.g: "::Mo1Do2Ri1V11V22::"
// int position: Position to get the site. The purpose of this variable is
// to plus with tempSite and from there, get two characters to know pass
// in get_type_enum() and return given site
// e.g: get 2 chars from (tempSite + 5) = "Do", get_type_enum("Do") = 3
// return 3
SiteType get_site(Path* myPath, int position, const char* tempSites);

// In this function, malloc myPath->sites variables, loop through the
// tempSites variable (explained above) with index += 3.
// In each loop: Check if: first and last positions are barrier
// The given site is valid or not
// If not, set myPath->valid = false and return
// Otherwise, set the capacity of each site if valid (0 < cap < 10).
// However, for barriers: capacity = numberOfPlayers
// If all of sites is valid, save all into struct Path
void get_sites(Path* myPath, const char* tempSites, int numberOfPlayers);

// in handle_path(), read the given number in pathFile, then malloc with
// appropriate sizes, loop through the path to read chars and save into
// save it into myPath->rawPath. After that, run get_sites()
void handle_path(FILE* pathFile, Path* myPath, int numberOfPlayers);

// Path is set up by handle_path() and its followings functions
// which means myPath->valid is already set up, then return myPath->valid
bool is_valid_path(char* path, Path* myPath, int numberOfPlayers);

// Based on the currentPositions, get the nearest barrier positions
// from myPath->sites and return its position
int nearest_barrier(Path* myPath, int currentPosition);

#endif