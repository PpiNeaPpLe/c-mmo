#include <stdio.h>
#include <string.h>
#include <stdlib.h> // random numbers
#include <time.h>   //time
#include <stdbool.h> // bool for god mode

// headers
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
    bool god_mode_enabled = false; // Flag for god mode

    // --- Argument Parsing --- 
    for (int i = 1; i < argc; ++i) { // Start from 1 to skip program name
        if (strcmp(argv[i], "-name") == 0) {
            if (i + 1 < argc) { // Make sure there's a name after the flag
                strncpy(playerName, argv[i + 1], MAX_NAME_LENGTH - 1);
                playerName[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
                printf("Starting game with player name: %s\n", playerName);
                name_set_from_args = 1;
                i++; // Skip the next argument (the name itself)
            } else {
                fprintf(stderr, "Warning: -name flag requires an argument.\n");
            }
        } else if (strcmp(argv[i], "-god") == 0) {
            god_mode_enabled = true;
            printf("GOD MODE ENABLED!\n");
        } else {
            fprintf(stderr, "Warning: Unknown argument '%s' ignored.\n", argv[i]);
        }
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
    initialize_player(&player, playerName, god_mode_enabled);
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
