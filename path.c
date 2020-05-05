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
    }
    if (!strcmp(site, "V1")) {
        return V1;
    }
    if (!strcmp(site, "V2")) {
        return V2;
    }
    if (!strcmp(site, "Do")) {
        return MONEY_2_POINT;
    }
    if (!strcmp(site, "Ri")) {
        return DRAW_NEXT_CARD;
    }
    if (!strcmp(site, "::")) {
        return BARRIER;
    }
    printf("Error Site Type\n");
    return ERROR_TYPE;
}

char* get_type_char(const int* site) {
    char* siteName = "";
    switch (*site) {
    case MONEY:
        siteName = "Mo";
        break;
    case V1:
        siteName = "V1";
        break;
    case V2:
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

void get_sites(Path* myPath, const char* tempSites,
        const int* numberOfPlayers) {
    char buffer[CHARS_OF_SITE];
    int numberOfSites = myPath->numberOfSites;
    int* available_capacity = malloc(sizeof(int) * numberOfSites);
    int** sites = malloc(sizeof(int*) * numberOfSites);
    for (int i = 0; i < numberOfSites; i++) {
        sites[i] = malloc (sizeof(int) * 2);
    }
    int position = 0;
        
    for (int i = 0; i < numberOfSites * CHARS_OF_SITE_AND_LIMIT; 
            i += CHARS_OF_SITE_AND_LIMIT) {
        int limitOfCurrentSite;
        memcpy(buffer, tempSites + i, CHARS_OF_SITE);
        SiteType s = get_type_enum(buffer);

        // both first site and last site must be barrier
        if (i == 0 || i == numberOfSites * CHARS_OF_SITE_AND_LIMIT -
                CHARS_OF_SITE_AND_LIMIT) {
                    if (s != BARRIER) {
                        myPath->valid = false;
                        return;
                    }
        }

        if (s == ERROR_TYPE) {
            myPath->valid = false;
            return;
        }
        
        if (tempSites[i] == ':') {
            limitOfCurrentSite = *numberOfPlayers;
        } else {
            limitOfCurrentSite = tempSites[i + 2] - '0';
            if (limitOfCurrentSite < 0 || limitOfCurrentSite > 9) {
                myPath->valid = false;
                return;
            }
        }

        int* currentSiteAndCapacity = malloc(sizeof(int) * 2);
        currentSiteAndCapacity[SITE] = s;
        currentSiteAndCapacity[CAPACITY] = limitOfCurrentSite;
        sites[position] = currentSiteAndCapacity;
        available_capacity[position] = limitOfCurrentSite;
        position++;
        // free(currentSiteAndCapacity);
    }
    myPath->available_capacity = available_capacity;
    myPath->valid = true;
    myPath->sites = sites;
}

void handle_path(FILE* pathFile, Path* myPath, const int* numberOfPlayers) {
    int numberOfSites;
    int next;

    fscanf(pathFile, "%d", &numberOfSites);
    // printf("Number: %d\n", numberOfSites);
    if (numberOfSites < 2 || fgetc(pathFile) != ';') {
        myPath->valid = false;
        return;
    }

    int numberOfChars = numberOfSites * CHARS_OF_SITE_AND_LIMIT;
    myPath->numberOfSites = numberOfSites;
    int digitsCount = count_number_digit(numberOfSites);
    int sizeOfPath = digitsCount + numberOfChars + 2;   //2 more space for
                                                        //'\n' and ';'
    myPath->sizeOfPath = sizeOfPath;

    char* rawPath = malloc(sizeof(char) * sizeOfPath);
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

    if (rawPath[sizeOfPath - 1] != '\n') {
        myPath->valid = false;
        return;
    }

    myPath->rawFile = rawPath;

    // rawSites does not include numberOfSites and ';'
    char* rawSites = rawPath + digitsCount + 1;
    get_sites(myPath, rawSites, numberOfPlayers);
}

bool is_valid_path_file(char* path, Path* myPath, const int* numberOfPlayers) {
    FILE* pathFile = fopen(path, "r");
    if (pathFile == NULL) {
        return false;
    }
    handle_path(pathFile, myPath, numberOfPlayers);
    fclose(pathFile);
    return myPath->valid;
}

int nearest_barrier(Path* myPath, const int* currentPosition) {
    int* numberOfSites = &myPath->numberOfSites;
    int** sites = myPath->sites;
    int nearestBarrier;
    for (int i = *currentPosition + 1; i < *numberOfSites; i++){
        int currentSite = sites[i][SITE];
        if (currentSite == BARRIER) {
            nearestBarrier = i;
            break;
        }
    }
    return nearestBarrier;
}