// player.c - Player related functions
#include "player.h"
#include <stdio.h>
#include <string.h>

// Initialize the player with a name and default stats
void initialize_player(Player *player, const char *name) {
    if (player == NULL || name == NULL) {
        fprintf(stderr, "Error: Cannot initialize player with NULL pointers.\n");
        return; // Or handle error appropriately
    }

    strncpy(player->name, name, MAX_NAME_LENGTH - 1);
    player->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
    
    // Default stats (can be adjusted later based on class)
    player->hp = 50;
    player->maxHp = 50;
    player->damage = 8;

    printf("Player %s created! HP: %d/%d, Damage: %d\n", 
           player->name, player->hp, player->maxHp, player->damage);
} 