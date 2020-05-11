#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <stdio.h>
#include "participants.h"

#define SITE 0
#define CAPACITY 1
#define ROW 1

#define READ_END 0
#define WRITE_END 1

#define DEFAULT_VALUE 0
#define DEFAULT_MONEY 7
#define EMPTY_VALUE -1

typedef enum Sites {
    MONEY = 0,
    V1 = 1,
    V2 = 2,
    MONEY_2_POINT = 3,
    DRAW_NEXT_CARD = 4,
    BARRIER = 5,
    ERROR_TYPE = 6
} SiteType;

typedef struct Path {
    int numberOfSites;
    int sizeOfPath;
    char* rawPath;
    int* availableCapacity;
    int** sites;
    bool valid;
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