#ifndef DECK_H
#define DECK_H

#include <stdbool.h>
#include <stdio.h>

typedef struct DeckFile {
    int numberOfCards;
    char* cards;
    bool valid;
} Deck;

typedef enum Cards {
    CARD_A = 1,
    CARD_B = 2,
    CARD_C = 3,
    CARD_D = 4,
    CARD_E = 5,
    CARD_ERROR = 6
} CardType;

// Take a character and return a relevent card type
// For example: Parameter is 'A' -> return CARD_A which is considered as 1
CardType get_card_enum(const char card);

// From file pointer of a deck file, read number of cards and cards from it
// if the card number is invalid, which is < 4 or any of cards in that file
// differ from A to E (inclusive), set myDeck->valid to false and return;
// Otherwise, save read data into myDeck->cards and myDeck->numberOfCards
void handle_deck(FILE* deckFile, Deck* myDeck);

// Parameters: file name of a deck file (e.g: "d1.deck"), struct Deck
// return whether true if valid file and false otherwise
bool is_valid_deck_file(char* deck, Deck* myDeck);

// Get next card from given Deck, starting at 0
// Go back at the beginning if the dealer run out of cards and keep going
char get_next_card(Deck* myDeck);

#endif