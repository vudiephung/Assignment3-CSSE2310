#include "deck.h"
#include "stdlib.h"

CardType get_card_enum(const char card) {                                                                       
    if (card == 'A') {
        return CARD_A;
    }
    if (card == 'B') {
        return CARD_B;
    }
    if (card == 'C') {
        return CARD_C;
    }
    if (card == 'D') {
        return CARD_D;
    }
    if (card == 'E') {
        return CARD_E;
    }
    return CARD_ERROR;
}

void handle_deck(FILE* deckFile, Deck *myDeck) {
    int next = 0;
    int numberOfCards;
    fscanf(deckFile, "%d", &numberOfCards);
    if (numberOfCards < 4) {
        myDeck->valid = false;
        return;
    }
    char* cards = malloc(sizeof(char) * (numberOfCards + 1));
    for (int i = 0; i < numberOfCards + 1; i++) {
        next = fgetc(deckFile);
        if ((next >= 'A' && next <= 'E') || next == '\n') {
            cards[i] = (char)next;
        } else {
            break;
        }
    }
    bool valid = (cards[numberOfCards] == '\n');
    if (valid) {
        myDeck->numberOfCards = numberOfCards;
        myDeck->cards = cards;
    }
    myDeck->valid = valid;
}

bool is_valid_deck_file(char* deck, Deck* myDeck) {
    FILE* deckFile = fopen(deck, "r");
    if (deckFile == NULL) {
        return false;
    }
    handle_deck(deckFile, myDeck);
    fclose(deckFile);
    return myDeck->valid;
}

char get_next_card(Deck* myDeck) {
    static int currentPos = 0;
    int numberOfCards = myDeck->numberOfCards;
    char* cards = myDeck->cards;
    if (currentPos == numberOfCards) {
        currentPos = 0;
    }
    return cards[currentPos++];
}