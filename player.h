// player.h - Header for player structure and functions
#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LENGTH 50

// Basic Player structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int hp;
    int maxHp;
    int damage;
    // We'll add class later
} Player;

// Function prototypes for player actions will go here
// For example:
// void initialize_player(Player *player);
void initialize_player(Player *player, const char *name);

#endif // PLAYER_H 