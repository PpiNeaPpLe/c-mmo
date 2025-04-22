// game.c - Game logic implementations
#include "game.h"
#include "player.h"
#include "enemy.h"
#include <stdio.h>
#include <stdlib.h> // For rand(), srand()
#include <time.h>   // For time()

// Handles the player's turn
void player_turn(Player *player, Enemy *enemy) {
    if (player == NULL || enemy == NULL) return;

    printf("Player %s's turn. Choose action (1: Attack): ", player->name);
    int choice;
    
    // Simple input handling - expects integer input
    if (scanf("%d", &choice) == 1) {
        // Clear the rest of the input buffer in case user typed more than just a number
        while (getchar() != '\n'); 

        if (choice == 1) {
            printf("%s attacks %s!\n", player->name, enemy->name);
            enemy->hp -= player->damage; // Apply damage
            printf("%s takes %d damage. Remaining HP: %d/%d\n", 
                   enemy->name, player->damage, enemy->hp, enemy->maxHp);
        }
        else {
            printf("Invalid action choice. Turn skipped.\n");
        }
    }
    else {
        // Handle non-integer input (e.g., user typed letters)
        printf("Invalid input. Please enter a number. Turn skipped.\n");
        // Clear the invalid input from the buffer
        while (getchar() != '\n'); 
    }
}

// Handles the enemy's turn (simple attack)
void enemy_turn(Player *player, Enemy *enemy) {
    if (player == NULL || enemy == NULL || enemy->hp <= 0) return;

    printf("\n%s's turn.\n", enemy->name);
    printf("%s attacks %s!\n", enemy->name, player->name);
    player->hp -= enemy->damage; // Apply damage
    printf("%s takes %d damage. Remaining HP: %d/%d\n", 
           player->name, enemy->damage, player->hp, player->maxHp);
} 