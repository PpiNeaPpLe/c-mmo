// Need these for printf, strings, random stuff, and time
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Needed for srand, rand
#include <time.h>   // Needed for time

// My other C files
#include "player.h"
#include "enemy.h"
#include "game.h"

int main(int argc, char *argv[])
{
    // Make the random numbers actually random (kinda)
    srand(time(NULL));

    Player player;
    Enemy enemy;

    char playerName[MAX_NAME_LENGTH];
    int name_set_from_args = 0; // Flag to see if we got name from args

    // Simple check for '-name' argument
    if (argc == 3 && strcmp(argv[1], "-name") == 0) {
        strncpy(playerName, argv[2], MAX_NAME_LENGTH - 1);
        playerName[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
        printf("Starting game with player name: %s\n", playerName);
        name_set_from_args = 1;
    }

    // Fancy title screen
    printf("====================================\n");
    printf("      Welcome to Simple RPG!\n");
    printf("====================================\n\n");
    
    // Ask for player name ONLY if not set by args
    if (!name_set_from_args) {
        printf("Enter your name, adventurer: ");
        if (fgets(playerName, sizeof(playerName), stdin) != NULL) {
            // Get rid of the newline character fgets leaves
            playerName[strcspn(playerName, "\n")] = '\0';
        } else {
            // Uh oh, couldn't read the name? just call them Hero lol
            strncpy(playerName, "Hero", MAX_NAME_LENGTH - 1);
            playerName[MAX_NAME_LENGTH - 1] = '\0'; // Make sure it's null-terminated
            printf("Could not read name, proceeding as 'Hero'.\n");
            // might need to clear stdin here? idk, probably fine
        }
    }
    
    // Story time!
    printf("\nGreetings, %s!\n\n", playerName);
    printf("Long ago, the Crown of Light kept the kingdom of Eldergarde in peace\n"
           "—until the paladin Malakar, consumed by greed, stole it and twisted its\n"
           "power into something dark. Now, the land withers. Monsters stalk the\n"
           "villages. And whispers speak of a shadow creeping from the Ashen Keep,\n"
           "where Malakar waits.\n\n"
           "You, a fledgling adventurer, arrive in Eldergarde with nothing but your\n"
           "wits and your steel. The villagers' eyes turn to you, desperate for hope.\n"
           "Will you answer the call?\n\n");
    
    // Make 'em hit Enter
    printf("(Press Enter to continue...)");
    // Eat the leftover newline from fgets if there was one
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); // Now wait for the real Enter press

    // --- Set up the fighters ---
    printf("\n--- Preparing for Battle! ---\n");
    initialize_player(&player, playerName);
    initialize_enemy(&enemy); 

    // --- Fight! ---
    printf("\n--- Combat Start! ---\n");
    int turn = 1;
    while (player.hp > 0 && enemy.hp > 0)
    {
        printf("\n--- Turn %d ---\n", turn);
        // Show HP before player goes
        printf("%s HP: %d/%d | %s HP: %d/%d\n", 
               player.name, player.hp, player.maxHp, 
               enemy.name, enemy.hp, enemy.maxHp);

        // Player's turn
        player_turn(&player, &enemy);
        
        // Did the player win?
        if (enemy.hp <= 0) break; // exit loop if enemy dead

        // Enemy's turn
        enemy_turn(&player, &enemy);

        turn++; // Next turn
    }

    // --- Who won? ---
    printf("\n--- Combat Over ---\n");
    if (player.hp <= 0)
    {
        printf("%s was defeated! Game Over.\n", player.name);
    }
    else // Enemy must be dead
    {
        printf("%s defeated %s! You Win!\n", player.name, enemy.name);
    }

    // --- Clean up ---
    // TODO: gotta free memory later if we use malloc
    cleanup_player(&player);
    // Enemy name is just text, no need to free it

    return 0; // We're done!
}
