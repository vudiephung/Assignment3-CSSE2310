#ifndef DECK_H
#define DECK_H

#include <stdbool.h>
#include <stdio.h>

typedef struct DeckFile {
    int numberOfCards;
    char* cards;
    bool valid;
} Deck;

typedef enum Cards{
    A = 1,
    B = 2,
    C = 3,
    D = 4,
    E = 5,
    ERROR_CARD = 6
} CardType;

CardType get_card_enum(const char card);

void handle_deck(FILE* deckFile, Deck *myDeck);

bool is_valid_deck_file(char* deck, Deck* myDeck);

#endif