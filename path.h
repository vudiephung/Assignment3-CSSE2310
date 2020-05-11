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
    char* rawFile;
    int sizeOfPath;
    int** sites;
    int* availableCapacity;
    bool valid;
} Path;

SiteType get_type_enum(const char* site);
char* get_type_char(const int* site);
SiteType get_site(Path* myPath, int i, const char* tempSites);
void get_sites(Path* myPath, const char* tempSites, int numberOfPlayers);
void handle_path(FILE* pathFile, Path* myPath, int numberOfPlayers);
bool is_valid_path(char* path, Path* myPath, int numberOfPlayers);
int nearest_barrier(Path* myPath, int currentPosition);

#endif