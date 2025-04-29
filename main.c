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
#include "save_game.h" // added save game header

// Global save filename for use across multiple functions
char saveFileName[MAX_FILENAME_LENGTH] = {0};

// prints game usage instructions
void print_usage(const char* program_name) {
    printf("\nUsage: %s [OPTIONS]\n", program_name);
    printf("\nAvailable options:\n");
    printf("  -name NAME       Set your character's name\n");
    printf("  -save FILENAME   Specify a save file to load or create\n");
    printf("  -god             Enable god mode (unlimited health & damage)\n");
    printf("  -log LEVEL       Set log level (bitfield: 0-15)\n");
    printf("                   1=errors, 2=combat, 4=debug, 8=funny, 15=all\n");
    printf("  -dif LEVEL       Set game difficulty\n");
    printf("                   0=easy, 1=normal, 2=hard\n");
    printf("  -difficulty LEVEL  Same as -dif\n");
    printf("  -nofun           Disable easter eggs and fun stuff\n");
    printf("  -new             Force start a new game (ignore saved game)\n");
    printf("  -help            Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -name Wizard -log 15 -dif 0\n", program_name);
    printf("  %s -save wizard.csv -god\n", program_name);
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
        {"-save", "-savefile", "-s"},
        {"-god", "-godmode", "-g"},
        {"-log", "-l", "-debug"},
        {"-dif", "-difficulty", "-d"},
        {"-nofun", "-boring", "-serious"},
        {"-help", "--help", "-h"},
        {"-new", "--new", "-newgame"}
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

    char playerName[MAX_NAME_LENGTH];
    int name_set_from_args = 0; // Flag to see if we got name from args
    bool god_mode_enabled = false; // Flag for god mode
    int log_level_override = -1;   // -1 means use environment
    bool show_help = false; // Flag to show help
    bool had_invalid_arg = false; // Flag to track invalid arguments
    bool force_new_game = false; // Flag to force starting a new game

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
        } else if (strcmp(arg, "-save") == 0) {
            if (i + 1 < argc) {
                strncpy(saveFileName, argv[i + 1], MAX_FILENAME_LENGTH - 1);
                saveFileName[MAX_FILENAME_LENGTH - 1] = '\0'; // Ensure null termination
                printf("Save file specified: %s\n", saveFileName);
                i++; // Skip the next argument (the filename)
            } else {
                fprintf(stderr, "Error: -save flag requires a filename argument.\n");
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
        } else if (strcmp(arg, "-new") == 0) {
            // Force starting a new game
            force_new_game = true;
            printf("Starting a new game (ignoring any saved game).\n");
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

    // --- Show Help and Exit if Requested ---
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }

    // If there were invalid arguments, show usage and return error
    if (had_invalid_arg) {
        printf("Use -help for more information on valid options.\n");
        return 1;
    }

    // --- Print Welcome Message ---
    printf("\n");
    printf("*************************************\n");
    printf("*      Welcome to C-MMO RPG!       *\n");
    printf("*************************************\n");
    printf("\n");

    // --- Get Player Name if Not Provided in Arguments ---
    if (!name_set_from_args) {
        printf("Enter your name (max %d chars): ", MAX_NAME_LENGTH - 1);
        if (fgets(playerName, MAX_NAME_LENGTH, stdin) == NULL) {
            // Error reading input
            fprintf(stderr, "Error reading name. Using default.\n");
            strcpy(playerName, "Unknown");
        } else {
            // Remove newline character if present
            playerName[strcspn(playerName, "\n")] = '\0';
            
            // Check if name is empty
            if (playerName[0] == '\0') {
                strcpy(playerName, "Unknown");
                printf("No name entered. Using 'Unknown'.\n");
            }
        }
    }
    
    // --- Check for saved game data ---
    bool should_load_save = false;
    // Only check for existing saves if we have a username and not forcing new game
    if (!force_new_game && playerName[0] != '\0') {
        // First check if a specific save file was provided
        if (saveFileName[0] != '\0') {
            should_load_save = save_game_exists(NULL, saveFileName);
            if (should_load_save) {
                printf("\nSave file '%s' found!\n", saveFileName);
                printf("Loading game automatically...\n");
            } else {
                printf("\nSave file '%s' not found. Starting new game.\n", saveFileName);
                // Will create this file when saving
            }
        } 
        // If no specific save was provided, check for default save with username
        else if (save_game_exists(playerName, NULL)) {
            printf("\nSaved game found for '%s'!\n", playerName);
            printf("Do you want to load your saved game?\n");
            
            // Get yes/no
            char response[10];
            printf("Load game? (y/n): ");
            if (fgets(response, sizeof(response), stdin) != NULL) {
                // Remove newline
                response[strcspn(response, "\n")] = '\0';
                
                // Convert to lowercase
                for (int i = 0; response[i]; i++) {
                    response[i] = tolower(response[i]);
                }
                
                if (response[0] == 'y') {
                    should_load_save = true;
                }
            }
        }
    }
    
    // --- Initialize Player ---
    if (should_load_save) {
        // Initialize with dummy name first, but don't prompt for class
        // Just create a basic player object that we'll overwrite with saved data
        player.name = strdup("temp"); // Temporary name that will be replaced
        player.inventory = NULL;      // Will be initialized by load_game
        player.inventory_size = 0;
        player.inventory_capacity = 0;
        
        // Then load saved data
        if (saveFileName[0] != '\0') {
            // Use the explicit save file if provided
            if (load_game(&player, saveFileName)) {
                printf("Game loaded successfully!\n");
            } else {
                // Fall back to new game if load fails
                printf("Failed to load game, starting new game instead.\n");
                if (player.name != NULL) {
                    free(player.name); // Free the temporary name
                    player.name = NULL;
                }
                initialize_player(&player, playerName, god_mode_enabled);
            }
        } else {
            // Otherwise try to load by username
            if (load_game(&player, NULL)) {
                printf("Game loaded successfully!\n");
            } else {
                // Fall back to new game if load fails
                printf("Failed to load game, starting new game instead.\n");
                if (player.name != NULL) {
                    free(player.name); // Free the temporary name
                    player.name = NULL;
                }
                initialize_player(&player, playerName, god_mode_enabled);
            }
        }
        
        // Set god mode if enabled in command line
        if (god_mode_enabled) {
            printf("God mode enabled for loaded character!\n");
            player.hp = 9999;
            player.maxHp = 9999;
            player.damage = 999;
        }
    } else {
        // Start new game
        initialize_player(&player, playerName, god_mode_enabled);
    }
    
    // --- Main Game Loop ---
    GameState game_state = GAME_STATE_MENU;
    
    while (game_state != GAME_STATE_GAME_OVER && game_state != GAME_STATE_WIN) {
        game_loop(&player, &game_state);
        
        // Check for game over condition
        if (player.hp <= 0) {
            printf("\n=== GAME OVER ===\n");
            printf("You have been defeated!\n");
            
            // Ask if they want to clear the save
            printf("Clear saved game? (y/n): ");
            char response[10];
            if (fgets(response, sizeof(response), stdin) != NULL) {
                // Remove newline
                response[strcspn(response, "\n")] = '\0';
                
                // Convert to lowercase
                for (int i = 0; response[i]; i++) {
                    response[i] = tolower(response[i]);
                }
                
                if (response[0] == 'y') {
                    clear_save(saveFileName[0] != '\0' ? saveFileName : NULL);
                }
            }
            
            game_state = GAME_STATE_GAME_OVER;
        }
    }
    
    // --- Cleanup ---
    cleanup_player(&player);
    
    return 0;
}
