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
    
    // Display class options
    printf("Choose your class:\n");
    printf("- Rogue\n");
    printf("- Mage\n");
    printf("- Paladin\n\n");
    
    // Get player name
    printf("Enter your name, adventurer: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = '\0';  // Remove newline character
    
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
    getchar();
    getchar(); // Wait for Enter press

    // --- Setup Player and Enemy (Placeholders) ---
    printf("\n--- Preparing for Battle! ---\n");
    // TODO: Implement proper player creation (e.g., choose class)
    strncpy(player.name, playerName, MAX_NAME_LENGTH - 1);
    player.name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
    player.hp = 50; // Example values
    player.maxHp = 50;
    player.damage = 8;
    printf("Player %s created! HP: %d, Damage: %d\n", player.name, player.hp, player.damage);

    // TODO: Implement proper enemy creation
    enemy.name = "Goblin"; // Simple enemy
    enemy.hp = 30;
    enemy.maxHp = 30;
    enemy.damage = 5;
    printf("A wild %s appears! HP: %d, Damage: %d\n", enemy.name, enemy.hp, enemy.damage);

    // --- Main Game Loop (Combat) ---
    printf("\n--- Combat Start! ---\n");
    int turn = 1;
    // Simple loop: continues as long as both player and enemy are alive
    while (player.hp > 0 && enemy.hp > 0)
    {
        printf("\n--- Turn %d ---\n", turn);
        printf("%s HP: %d/%d | %s HP: %d/%d\n", player.name, player.hp, player.maxHp, enemy.name, enemy.hp, enemy.maxHp);

        // == Player's Turn (Placeholder) ==
        printf("Player %s's turn. Choose action (1: Attack): ", player.name);
        int choice;
        // VERY basic input handling - assumes user enters '1'
        if (scanf("%d", &choice) == 1 && choice == 1)
        {
            printf("%s attacks %s!\n", player.name, enemy.name);
            // TODO: Implement attack logic in game.c
            enemy.hp -= player.damage; // Simple damage calculation
            printf("%s takes %d damage.\n", enemy.name, player.damage);
        }
        else
        {
            // Clear input buffer if invalid input
            while (getchar() != '\n'); 
            printf("Invalid action. Turn skipped.\n");
        }
        
        // Check if enemy is defeated after player's turn
        if (enemy.hp <= 0) break;

        // == Enemy's Turn (Placeholder) ==
        printf("\n%s's turn.\n", enemy.name);
        printf("%s attacks %s!\n", enemy.name, player.name);
        // TODO: Implement enemy attack logic in game.c
        player.hp -= enemy.damage; // Simple damage calculation
        printf("%s takes %d damage.\n", player.name, enemy.damage);

        turn++;
    }

    // --- Combat End --- 
    printf("\n--- Combat Over ---\n");
    if (player.hp <= 0)
    {
        printf("%s was defeated! Game Over.\n", player.name);
    }
    else
    {
        printf("%s defeated %s! You Win!\n", player.name, enemy.name);
    }

    return 0;
}
