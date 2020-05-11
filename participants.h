#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

// From A to E (inclusively), there are 5 characters
#define NUM_A_TO_E 5

// struct Participant stores the data of all players
typedef struct {
    int numberOfPlayers;    // based on argc (dealer) or argv (player)
    int nextTurn;           // When dealer determine who should go next,
                            // its playerId will be stored in here
    int nextCard;           // the card that dealer picked from a deck
    int** positions;        // positions of all players can be considered as
                            // positions[playerId][0]: Store which row that
                            // player is at
                            // positions[playerId][1]: Store which site
                            // (0 <= site < numberOfSites) that player is at
                            // e.g: There are 3 players
                            // At beginning of the game,
                            // positions[2][0] = 0, positions[2][1] = 0
                            // positions[0][0] = 2, positions[0][1] = 0
    int* sizes;             // Store how many players at each site
                            // e.g: 3 players at the first barrier
                            // sizes[0] = 3 (0 <= size <= CAPACITY)
    int* moneys;            // Money of all players
                            // e.g: 3 players. At the beginning of the game
                            // moneys = [7,7,7];
    int* points;            // Points of all players, all 0 when the game
                            // starts
    int* siteV1;            // How many V1 site each player went by
                            // e.g: 3 players siteV1 = [0,0,0]
    int* siteV2;            // Same idea with siteV1
    int** cards;            // cards[playerId][letters]
                            // e.g: 1 player
                            // cards[0] = [1,2,3,4,5]
                            // player 0 has 15 cards, 4 card D
    int* pointChange;       // How many points change in each move?
    int* moneyChange;       // How much money change in each move?
    int* nextMove;          // Desire move of all players (get via "DO")
} Participant;

#endif