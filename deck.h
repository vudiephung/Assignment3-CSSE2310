#ifndef DECK_H
#define DECK_H

#include <stdbool.h>
#include <stdio.h>

// Struct DeckFile defines needed variable for a given Deck
typedef struct DeckFile {   // e.g: 5ADBEC
    int numberOfCards;      // Number of cards in deck file: 5
    char* cards;            // Array of type char stores cards in it:
                            // ['A', 'D', 'B', 'E', 'C']
    bool valid;             // true iff valid Deck file
} Deck;

// Save Card characters as a number -> easier to store in array later
typedef enum Cards {
    CARD_A = 1,
    CARD_B = 2,
    CARD_C = 3,
    CARD_D = 4,
    CARD_E = 5,
    CARD_ERROR = 6
} CardType;

CardType get_card_enum(const char card);

void handle_deck(FILE* deckFile, Deck* myDeck);

bool is_valid_deck_file(char* deck, Deck* myDeck);

char get_next_card(Deck* myDeck);

#endif