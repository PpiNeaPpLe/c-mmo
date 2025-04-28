#include <stdio.h>
#include <string.h>
#include <stdlib.h> // random numbers
#include <time.h>   //time
#include <stdbool.h> // bool for god mode

// headers
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "utils.h" // added utils header

int main(int argc, char *argv[])
{
    // Make the random numbers actually random (kinda)
    srand(time(NULL));

    Player player;
    Enemy enemy;

    char playerName[MAX_NAME_LENGTH];
    int name_set_from_args = 0; // Flag to see if we got name from args
    bool god_mode_enabled = false; // Flag for god mode
    int log_level_override = -1;   // -1 means use environment

    // Initialize environment variables first thing
    setup_env_variables();

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
        } else if (strcmp(argv[i], "-log") == 0) {
            // New option for log level
            if (i + 1 < argc) {
                // Try to get log level as number
                log_level_override = atoi(argv[i + 1]);
                printf("Log level set to: 0x%X\n", log_level_override);
                i++; // Skip the level argument
                
                // Set the environment variable for other modules
                char env_var[32];
                sprintf(env_var, "%d", log_level_override);
                setenv("GAME_LOG_LEVEL", env_var, 1); // 1 means override
                
                // Reinitialize environment to apply changes
                setup_env_variables();
            } else {
                fprintf(stderr, "Warning: -log flag requires a numeric argument.\n");
            }
        } else if (strcmp(argv[i], "-dif") == 0 || strcmp(argv[i], "-difficulty") == 0) {
            // Difficulty option
            if (i + 1 < argc) {
                int difficulty = atoi(argv[i + 1]);
                if (difficulty >= 0 && difficulty <= 2) {
                    char env_var[32];
                    sprintf(env_var, "%d", difficulty);
                    setenv("GAME_DIFFICULTY", env_var, 1);
                    
                    const char* dif_name = "normal";
                    if (difficulty == 0) dif_name = "easy";
                    else if (difficulty == 2) dif_name = "hard";
                    
                    printf("Difficulty set to: %s (%d)\n", dif_name, difficulty);
                    i++; // Skip the difficulty value
                    
                    // Reinitialize environment to apply changes
                    setup_env_variables();
                } else {
                    fprintf(stderr, "Warning: Difficulty must be 0 (easy), 1 (normal), or 2 (hard).\n");
                }
            } else {
                fprintf(stderr, "Warning: -difficulty flag requires an argument.\n");
            }
        } else if (strcmp(argv[i], "-nofun") == 0) {
            // Disable Easter eggs
            setenv("GAME_EASTER_EGGS", "0", 1);
            printf("Easter eggs disabled. Boring mode activated.\n");
            
            // Reinitialize environment to apply changes
            setup_env_variables();
        } else {
            fprintf(stderr, "Warning: Unknown argument '%s' ignored.\n", argv[i]);
        }
    }

    // Show command line help if the debug log level is enabled
    if (is_logging_enabled(LOG_DEBUG)) {
        log_event(LOG_DEBUG, "Command line options:");
        log_event(LOG_DEBUG, "  -name NAME     Set player name");
        log_event(LOG_DEBUG, "  -god           Enable god mode");
        log_event(LOG_DEBUG, "  -log LEVEL     Set log level (bitfield)");
        log_event(LOG_DEBUG, "  -dif LEVEL     Set difficulty (0=easy, 1=normal, 2=hard)");
        log_event(LOG_DEBUG, "  -nofun         Disable Easter eggs");
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
    
    // Show environment variables if debug is enabled
    if (is_logging_enabled(LOG_DEBUG)) {
        const char* difficulty_env = getenv("GAME_DIFFICULTY");
        const char* easter_eggs_env = getenv("GAME_EASTER_EGGS");
        const char* log_level_env = getenv("GAME_LOG_LEVEL");
        
        log_event(LOG_DEBUG, "Environment variables set:");
        log_event(LOG_DEBUG, "  GAME_DIFFICULTY=%s", difficulty_env ? difficulty_env : "not set");
        log_event(LOG_DEBUG, "  GAME_EASTER_EGGS=%s", easter_eggs_env ? easter_eggs_env : "not set");
        log_event(LOG_DEBUG, "  GAME_LOG_LEVEL=%s", log_level_env ? log_level_env : "not set");
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

        // Log start of turn with our variadic function
        log_event(LOG_DEBUG, "Starting turn %d", turn);

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
        log_event(LOG_COMBAT, "Game over: %s was defeated!", player.name);
    }
    else // Enemy must be dead
    {
        printf("%s defeated %s! You Win!\n", player.name, enemy.name);
        log_event(LOG_COMBAT, "Victory: %s defeated %s!", player.name, enemy.name);
    }

    // --- Clean up ---
    // TODO: gotta free memory later if we use malloc
    cleanup_player(&player);
    // Enemy name is just text, no need to free it

    return 0; // We're done!
}
