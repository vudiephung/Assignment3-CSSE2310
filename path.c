#include "path.h"
#include "game.h"
#include "participants.h"
#include "player.h"
#include "utils.h"
#include "stdlib.h"
#include "string.h"

#define CHARS_OF_SITE 2 // e.g: Mo has 2 chars
#define CHARS_OF_SITE_AND_LIMIT 3 // e.g: Mo3 ::- has 3 chars

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

void get_sites(Path* myPath, const char* tempSites, int numberOfPlayers) {
    int numberOfSites = myPath->numberOfSites;
    int* availableCapacity = malloc(sizeof(int) * numberOfSites);
    int** sites = malloc(sizeof(int*) * numberOfSites);
    for (int i = 0; i < numberOfSites; i++) {
        sites[i] = malloc (sizeof(int) * 2);
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
        if (position == 0 || position == numberOfSites * CHARS_OF_SITE_AND_LIMIT -
            CHARS_OF_SITE_AND_LIMIT) {
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
    while(numberOfSites) {
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

bool is_valid_path(char* path, Path* myPath, int numberOfPlayers) {
    FILE* pathFile = fopen(path, "r");
    if (pathFile == NULL) {
        return false;
    }
    handle_path(pathFile, myPath, numberOfPlayers);
    fclose(pathFile);
    return myPath->valid;
}

int nearest_barrier(Path* myPath, int currentPosition) {
    int* numberOfSites = &myPath->numberOfSites;
    int** sites = myPath->sites;
    int nearestBarrier;
    for (int i = currentPosition + 1; i < *numberOfSites; i++){
        int currentSite = sites[i][SITE];
        if (currentSite == BARRIER) {
            nearestBarrier = i;
            break;
        }
    }
    return nearestBarrier;
}