#ifndef DECK_H
#define DECK_H

#include <stdbool.h>
#include <stdio.h>

typedef struct DeckFile {
    int numberOfCards;
    char* cards;
    bool valid;
} Deck;

// get_type_enum(const char* site);

void handle_deck(FILE* deckFile, Deck *myDeck);

bool is_valid_deck_file(char* deck, Deck* myDeck);

#endif