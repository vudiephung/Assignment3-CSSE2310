#include <stdlib.h>
#include <string.h>
#include "path.h"
#include "game.h"
#include "participants.h"
#include "player.h"
#include "utils.h"

#define CHARS_OF_SITE 2 // e.g: Mo has 2 chars
#define CHARS_OF_SITE_AND_LIMIT 3 // e.g: Mo3 ::- has 3 chars

// return corresponding SiteType of the parameter char* site
// e.g: return MONEY (0) iff site == "Mo"
SiteType get_type_enum(const char* site) {
    //Mo, V1, V2, Do, Ri, ::
    if (!strcmp(site, "Mo")) {
        return MONEY;
    } else if (!strcmp(site, "V1")) {
        return SITE_V1;
    } else if (!strcmp(site, "V2")) {
        return SITE_V2;
    } else if (!strcmp(site, "Do")) {
        return MONEY_2_POINT;
    } else if (!strcmp(site, "Ri")) {
        return DRAW_NEXT_CARD;
    } else if (!strcmp(site, "::")) {
        return BARRIER;
    } else {
        return ERROR_TYPE;
    }
}

// With given SiteType, return corresponding char*
// e.g: get_type_char(5) == "::"
char* get_type_char(SiteType site) {
    char* siteName = "";
    switch (site) {
        case MONEY:
            siteName = "Mo";
            break;
        case SITE_V1:
            siteName = "V1";
            break;
        case SITE_V2:
            siteName = "V2";
            break;
        case MONEY_2_POINT:
            siteName = "Do";
            break;
        case DRAW_NEXT_CARD:
            siteName = "Ri";
            break;
        case BARRIER:
            siteName = "::";
            break; 
        default:
            break;
    }
    return siteName;
}

// Parameters: char* tempSite is a path that does not include number and ';'
// e.g: "::Mo1Do2Ri1V11V22::"
// int position: Position to get the site. The purpose of this variable is
// to plus with tempSite and from there, get two characters to know pass
// in get_type_enum() and return given site
// e.g: get 2 chars from (tempSite + 5) = "Do", get_type_enum("Do") = 3
// return 3
SiteType get_site(Path* myPath, int position, const char* tempSites) {
    SiteType site;
    char buffer[CHARS_OF_SITE + 1];     
    for (int j = 0; j < CHARS_OF_SITE; j++) {
        buffer[j] = (tempSites + position)[j];
    }
    buffer[CHARS_OF_SITE] = '\0';
    site = get_type_enum(buffer);
    return site;
}

// In this function, loop through the
// tempSites variable (explained above) with index += 3.
// In each loop: Check if first and last positions are barrier
// The given site is valid or not
// If not, set myPath->valid = false and return
// Otherwise, set the capacity of each site if valid (0 < cap < 10).
// However, for barriers: capacity = numberOfPlayers
// If all of sites is valid, save all into struct Path
// return void;
void get_sites(Path* myPath, const char* tempSites, int numberOfPlayers) {
    int numberOfSites = myPath->numberOfSites;
    int* availableCapacity = malloc(sizeof(int) * numberOfSites);
    int** sites = malloc(sizeof(int*) * numberOfSites);
    for (int i = 0; i < numberOfSites; i++) {
        sites[i] = malloc(sizeof(int) * 2);
    }
    int count = 0;
        
    for (int position = 0; position < numberOfSites * CHARS_OF_SITE_AND_LIMIT; 
            position += CHARS_OF_SITE_AND_LIMIT) {
        int limitOfCurrentSite;
        SiteType site = get_site(myPath, position, tempSites);
        if (site == ERROR_TYPE) {
            myPath->valid = false;
            return;
        }
        // both first site and last site must be barrier
        if (position == 0 || position == numberOfSites *
                CHARS_OF_SITE_AND_LIMIT - CHARS_OF_SITE_AND_LIMIT) {
            if (site != BARRIER) {
                myPath->valid = false;
                return;
            }
        }
        if (tempSites[position] == ':') {
            limitOfCurrentSite = numberOfPlayers;
        } else {
            limitOfCurrentSite = tempSites[position + 2] - '0';
            if (limitOfCurrentSite < 0 || limitOfCurrentSite > 9) {
                myPath->valid = false;
                return;
            }
        }
        int* currentSiteAndCapacity = malloc(sizeof(int) * 2);
        currentSiteAndCapacity[SITE] = site;
        currentSiteAndCapacity[CAPACITY] = limitOfCurrentSite;
        sites[count] = currentSiteAndCapacity;
        availableCapacity[count] = limitOfCurrentSite;
        count++;
    }
    myPath->availableCapacity = availableCapacity;
    myPath->valid = true;
    myPath->sites = sites;
}

// in handle_path(), read the given number in pathFile, then malloc with
// appropriate sizes, loop through the path to read chars and save into
// save it into myPath->rawPath. After that, run get_sites()
// return void
void handle_path(FILE* pathFile, Path* myPath, int numberOfPlayers) {
    int numberOfSites;
    int next;

    fscanf(pathFile, "%d", &numberOfSites);
    if (numberOfSites < 2 || fgetc(pathFile) != ';') {
        myPath->valid = false;
        return;
    }

    myPath->numberOfSites = numberOfSites;
    int digitsCount = count_number_digit(numberOfSites);
    int numberOfChars = numberOfSites * CHARS_OF_SITE_AND_LIMIT;
    // 2 more space for ';' and '\n'
    int sizeOfPath = digitsCount + numberOfChars + 2;

    char* rawPath = malloc(sizeof(char) * (sizeOfPath + 1));
    int pos = digitsCount - 1;
    while (numberOfSites) {
        rawPath[pos--] = (numberOfSites % 10) + '0';
        numberOfSites /= 10;
    }
    rawPath[digitsCount] = ';';
    for (int i = digitsCount + 1; i < sizeOfPath; i++) {
        next = fgetc(pathFile);
        rawPath[i] = (char)next;
    }

    myPath->rawPath = rawPath;

    // rawSites does not include numberOfSites and ';'
    char* rawSites = rawPath + digitsCount + 1;
    get_sites(myPath, rawSites, numberOfPlayers);
}

// Path is set up by handle_path() and its followings functions
// which means myPath->valid is already set up, then return myPath->valid
// return void
bool is_valid_path(char* path, Path* myPath, int numberOfPlayers) {
    FILE* pathFile = fopen(path, "r");
    if (pathFile == NULL) {
        return false;
    }
    handle_path(pathFile, myPath, numberOfPlayers);
    fclose(pathFile);
    return myPath->valid;
}

// Based on the currentPositions, get the nearest barrier positions
// from myPath->sites and return its position
int nearest_barrier(Path* myPath, int currentPosition) {
    int* numberOfSites = &myPath->numberOfSites;
    int** sites = myPath->sites;
    int nearestBarrier;
    for (int i = currentPosition + 1; i < *numberOfSites; i++) {
        if (sites[i][SITE] == BARRIER) {
            nearestBarrier = i;
            break;
        }
    }
    return nearestBarrier;
}