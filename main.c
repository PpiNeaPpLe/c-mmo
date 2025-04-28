#include <stdio.h>
#include <string.h>
#include <stdlib.h> // random numbers
#include <time.h>   //time
#include <stdbool.h> // bool for god mode
#include <ctype.h>  // for tolower

// headers
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "utils.h" // added utils header

// prints game usage instructions
void print_usage(const char* program_name) {
    printf("\nUsage: %s [OPTIONS]\n", program_name);
    printf("\nAvailable options:\n");
    printf("  -name NAME       Set your character's name\n");
    printf("  -god             Enable god mode (unlimited health & damage)\n");
    printf("  -log LEVEL       Set log level (bitfield: 0-15)\n");
    printf("                   1=errors, 2=combat, 4=debug, 8=funny, 15=all\n");
    printf("  -dif LEVEL       Set game difficulty\n");
    printf("                   0=easy, 1=normal, 2=hard\n");
    printf("  -difficulty LEVEL  Same as -dif\n");
    printf("  -nofun           Disable easter eggs and fun stuff\n");
    printf("  -help            Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -name Wizard -log 15 -dif 0\n", program_name);
    printf("  %s -god -nofun\n", program_name);
    printf("\n");
}

// Helper function to check if a string starts with a prefix, case insensitive
bool starts_with_insensitive(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) return false;
    
    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);
    
    if (str_len < prefix_len) return false;
    
    for (size_t i = 0; i < prefix_len; i++) {
        if (tolower((unsigned char)str[i]) != tolower((unsigned char)prefix[i])) {
            return false;
        }
    }
    
    return true;
}

// Find the closest matching parameter for a given input
const char* find_closest_param(const char* input) {
    // Handle the special case for -fart explicitly
    if (strcmp(input, "-fart") == 0 || 
        strcmp(input, "-FART") == 0 || 
        starts_with_insensitive(input, "-fart")) {
        return "FART";  // Special return value for fart
    }
    
    // Common parameters and their typos/variants
    const char* known_params[][3] = {
        {"-name", "-n", "-player"},
        {"-god", "-godmode", "-g"},
        {"-log", "-l", "-debug"},
        {"-dif", "-difficulty", "-d"},
        {"-nofun", "-boring", "-serious"},
        {"-help", "--help", "-h"}
    };
    
    const int num_param_groups = sizeof(known_params) / sizeof(known_params[0]);
    
    // Try to find an exact match first
    for (int i = 0; i < num_param_groups; i++) {
        for (int j = 0; j < 3; j++) {
            if (known_params[i][j][0] != '\0' && strcmp(input, known_params[i][j]) == 0) {
                return known_params[i][0]; // Return the canonical form
            }
        }
    }
    
    // Try to find a prefix match (for things like -godm instead of -godmode)
    for (int i = 0; i < num_param_groups; i++) {
        for (int j = 0; j < 3; j++) {
            if (known_params[i][j][0] != '\0' && starts_with_insensitive(input, known_params[i][j])) {
                return known_params[i][0]; // Return the canonical form
            }
        }
    }
    
    // Handle some special cases with common typos
    if (starts_with_insensitive(input, "-go") || 
        starts_with_insensitive(input, "-gm")) {
        return "-god";
    }
    
    if (starts_with_insensitive(input, "-di") || 
        starts_with_insensitive(input, "-diff")) {
        return "-dif";
    }
    
    if (starts_with_insensitive(input, "-no") || 
        starts_with_insensitive(input, "-nf")) {
        return "-nofun";
    }
    
    // No close match found
    return NULL;
}

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
    bool show_help = false; // Flag to show help
    bool had_invalid_arg = false; // Flag to track invalid arguments

    // Initialize environment variables first thing
    setup_env_variables();

    // --- Argument Parsing --- 
    for (int i = 1; i < argc; ++i) { // Start from 1 to skip program name
        const char* arg = argv[i];
        const char* canonical_arg = NULL;
        
        // Check if it's a known parameter (with exact match)
        if (arg[0] == '-') {
            canonical_arg = find_closest_param(arg);
            
            // If we found a close match but it's not an exact match, suggest it
            if (canonical_arg != NULL && strcmp(canonical_arg, arg) != 0) {
                printf("Note: Treating '%s' as '%s'\n", arg, canonical_arg);
                arg = canonical_arg; // Use the canonical form
            }
        }
        
        // Now process the argument (either original or canonical form)
        if (strcmp(arg, "-name") == 0) {
            if (i + 1 < argc) { // Make sure there's a name after the flag
                strncpy(playerName, argv[i + 1], MAX_NAME_LENGTH - 1);
                playerName[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
                printf("Starting game with player name: %s\n", playerName);
                name_set_from_args = 1;
                i++; // Skip the next argument (the name itself)
            } else {
                fprintf(stderr, "Error: -name flag requires an argument.\n");
                had_invalid_arg = true;
            }
        } else if (strcmp(arg, "-god") == 0) {
            god_mode_enabled = true;
            printf("GOD MODE ENABLED!\n");
        } else if (strcmp(arg, "-log") == 0) {
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
                fprintf(stderr, "Error: -log flag requires a numeric argument (0-15).\n");
                had_invalid_arg = true;
            }
        } else if (strcmp(arg, "-dif") == 0 || strcmp(arg, "-difficulty") == 0) {
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
                    fprintf(stderr, "Error: Difficulty must be 0 (easy), 1 (normal), or 2 (hard).\n");
                    had_invalid_arg = true;
                }
            } else {
                fprintf(stderr, "Error: -difficulty flag requires an argument (0-2).\n");
                had_invalid_arg = true;
            }
        } else if (strcmp(arg, "-nofun") == 0) {
            // Disable Easter eggs
            setenv("GAME_EASTER_EGGS", "0", 1);
            printf("Easter eggs disabled. Boring mode activated.\n");
            
            // Reinitialize environment to apply changes
            setup_env_variables();
        } else if (strcmp(arg, "-help") == 0 || strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            // Show help
            show_help = true;
        } else if (canonical_arg != NULL && strcmp(canonical_arg, "FART") == 0) {
            // Handle the special case for -fart
            printf("💨 PFFFFFFTTTtttt! What's that smell?\n");
            printf("Sorry, '-fart' is not a valid option. Did you mean:\n");
            printf("  -dif     (set difficulty)\n");
            printf("  -god     (enable god mode)\n");
            printf("  -help    (show help menu)\n");
            
            // Add a joke environment variable
            setenv("GAME_FART", "true", 1);
            
            // Enable funny logs for fart jokes
            int current_log = get_env_int("GAME_LOG_LEVEL", 0);
            char env_var[32];
            sprintf(env_var, "%d", current_log | LOG_FUNNY);
            setenv("GAME_LOG_LEVEL", env_var, 1);
            setup_env_variables();
            
            had_invalid_arg = true;
        } else if (arg[0] == '-') {
            // Unknown parameter that starts with -
            fprintf(stderr, "Error: Unknown argument '%s'.\n", arg);
            
            // Try to find a similar parameter to suggest
            const char* suggestion = find_closest_param(arg);
            if (suggestion != NULL && strcmp(suggestion, "FART") != 0) {
                fprintf(stderr, "Did you mean '%s'?\n", suggestion);
            }
            
            had_invalid_arg = true;
        } else {
            // Not a parameter (doesn't start with -)
            fprintf(stderr, "Error: Unexpected argument '%s'.\n", arg);
            had_invalid_arg = true;
        }
    }

    // Handle funny or nonsensical inputs with Easter eggs if enabled
    if (is_logging_enabled(LOG_FUNNY)) {
        // Check environment for joke variables
        const char* joke_var = getenv("GAME_FART");
        if (joke_var != NULL) {
            printf("💨 PFFFFFFTTTtttt! Someone set GAME_FART=%s! Gross!\n", joke_var);
        }
    }

    // If there was an invalid argument or help was requested, show usage
    if (had_invalid_arg || show_help) {
        print_usage(argv[0]);
        
        // If help was explicitly requested, exit with success
        // Otherwise exit with error code
        if (show_help && !had_invalid_arg) {
            return 0;
        } else if (had_invalid_arg) {
            fprintf(stderr, "Run with -help for more information.\n");
            return 1;
        }
    }

    // Show command line help if the debug log level is enabled
    if (is_logging_enabled(LOG_DEBUG)) {
        log_event(LOG_DEBUG, "Command line options detected:");
        if (name_set_from_args) {
            log_event(LOG_DEBUG, "  Name: %s", playerName);
        }
        if (god_mode_enabled) {
            log_event(LOG_DEBUG, "  God mode: enabled");
        }
        log_event(LOG_DEBUG, "  Log level: 0x%X", get_env_int("GAME_LOG_LEVEL", 0));
        log_event(LOG_DEBUG, "  Difficulty: %d", get_env_int("GAME_DIFFICULTY", 1));
        log_event(LOG_DEBUG, "  Easter eggs: %s", 
                 get_env_bool("GAME_EASTER_EGGS", true) ? "enabled" : "disabled");
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
