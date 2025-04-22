#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Needed for srand, rand
#include <time.h>   // Needed for time

// Include our new headers
#include "player.h"
#include "enemy.h"
#include "game.h"

int main() 
{
    // Seed random number generator (useful for combat later)
    srand(time(NULL));

    Player player;
    Enemy enemy;

    char playerName[MAX_NAME_LENGTH];

    // Display game title and welcome message
    printf("====================================\n");
    printf("      Welcome to Simple RPG!\n");
    printf("====================================\n\n");
    
    // Get player name
    printf("Enter your name, adventurer: ");
    if (fgets(playerName, sizeof(playerName), stdin) != NULL) {
        playerName[strcspn(playerName, "\n")] = '\0';
    } else {
        // Handle error reading name, e.g., use a default name
        strncpy(playerName, "Hero", MAX_NAME_LENGTH - 1);
        playerName[MAX_NAME_LENGTH - 1] = '\0';
        printf("Could not read name, proceeding as 'Hero'.\n");
        // We might need to clear stdin here if fgets failed mid-read
        // but for simplicity, we'll assume it failed cleanly or got EOF
    }
    
    // Display story introduction
    printf("\nGreetings, %s!\n\n", playerName);
    printf("Long ago, the Crown of Light kept the kingdom of Eldergarde in peace\n"
           "—until the paladin Malakar, consumed by greed, stole it and twisted its\n"
           "power into something dark. Now, the land withers. Monsters stalk the\n"
           "villages. And whispers speak of a shadow creeping from the Ashen Keep,\n"
           "where Malakar waits.\n\n"
           "You, a fledgling adventurer, arrive in Eldergarde with nothing but your\n"
           "wits and your steel. The villagers' eyes turn to you, desperate for hope.\n"
           "Will you answer the call?\n\n");
    
    // Wait for user to continue
    printf("(Press Enter to continue...)");
    // Consume potential leftover newline from fgets before waiting for Enter
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); // Wait for Enter press

    // --- Setup Player and Enemy --- 
    printf("\n--- Preparing for Battle! ---\n");
    initialize_player(&player, playerName);
    initialize_enemy(&enemy); 

    // --- Main Game Loop (Combat) ---
    printf("\n--- Combat Start! ---\n");
    int turn = 1;
    while (player.hp > 0 && enemy.hp > 0)
    {
        printf("\n--- Turn %d ---\n", turn);
        // Display HP before the player acts
        printf("%s HP: %d/%d | %s HP: %d/%d\n", 
               player.name, player.hp, player.maxHp, 
               enemy.name, enemy.hp, enemy.maxHp);

        // Player's Turn
        player_turn(&player, &enemy);
        
        // Check if enemy defeated after player's turn
        if (enemy.hp <= 0) break;

        // Enemy's Turn
        enemy_turn(&player, &enemy);

        turn++;
    }

    // --- Combat End --- 
    printf("\n--- Combat Over ---\n");
    if (player.hp <= 0)
    {
        printf("%s was defeated! Game Over.\n", player.name);
    }
    else // Enemy must have hp <= 0
    {
        printf("%s defeated %s! You Win!\n", player.name, enemy.name);
    }

    // --- Cleanup --- 
    // IMPORTANT: Free allocated memory before exiting
    cleanup_player(&player);
    // Note: Enemy name was a string literal, no need to free enemy struct fields

    return 0;
}
