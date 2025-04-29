#include "save_game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

// Helper function to ensure the save directory exists
static bool ensure_save_directory() {
    // Create save directory if it doesn't exist
    #ifdef _WIN32
    // Windows
    if (mkdir(DEFAULT_SAVE_DIR) != 0 && errno != EEXIST) {
        printf("Warning: Could not create save directory\n");
        return false;
    }
    #else
    // Unix-like systems
    if (mkdir(DEFAULT_SAVE_DIR, 0755) != 0 && errno != EEXIST) {
        printf("Warning: Could not create save directory\n");
        return false;
    }
    #endif
    return true;
}

// Get the full path for the save file
void get_save_filename(char *result, const char *username, const char *filename) {
    if (result == NULL) {
        return;
    }
    
    // Initialize the result buffer
    result[0] = '\0';
    
    // Ensure save directory exists
    ensure_save_directory();
    
    // Construct the filename
    if (filename != NULL) {
        // Use the provided filename
        snprintf(result, MAX_FILENAME_LENGTH, "%s/%s", DEFAULT_SAVE_DIR, filename);
    } else if (username != NULL) {
        // Use the username.csv
        snprintf(result, MAX_FILENAME_LENGTH, "%s/%s.csv", DEFAULT_SAVE_DIR, username);
    } else {
        // Fallback to default
        snprintf(result, MAX_FILENAME_LENGTH, "%s/default.csv", DEFAULT_SAVE_DIR);
    }
}

// Save player stats to a CSV file
bool save_game(Player *player, const char *filename) {
    if (player == NULL) {
        printf("Error: Can't save NULL player\n");
        return false;
    }
    
    char save_path[MAX_FILENAME_LENGTH];
    get_save_filename(save_path, player->name, filename);
    
    FILE *file = fopen(save_path, "w");
    if (file == NULL) {
        printf("Error: Could not open save file '%s'\n", save_path);
        return false;
    }
    
    // Write CSV header
    fprintf(file, "key,value\n");
    
    // Write basic player stats
    fprintf(file, "NAME,%s\n", player->name);
    fprintf(file, "CLASS,%d\n", player->playerClass);
    fprintf(file, "HP,%d\n", player->hp);
    fprintf(file, "MAX_HP,%d\n", player->maxHp);
    fprintf(file, "DAMAGE,%d\n", player->damage);
    
    // Write progression stats
    fprintf(file, "XP,%d\n", player->xp);
    fprintf(file, "LEVEL,%d\n", player->level);
    fprintf(file, "KILLS,%d\n", player->kills);
    fprintf(file, "GOLD,%d\n", player->gold);
    fprintf(file, "AREA,%d\n", player->area_level);
    
    // Write status flags
    fprintf(file, "IS_POISONED,%d\n", (int)player->is_poisoned);
    fprintf(file, "IS_SHIELDED,%d\n", (int)player->is_shielded);
    fprintf(file, "TURN_SKIPPED,%d\n", (int)player->turn_skipped);
    
    // Write timestamp
    fprintf(file, "TIMESTAMP,%ld\n", (long)time(NULL));
    
    fclose(file);
    
    printf("Game saved successfully for %s (Level %d) to '%s'!\n", 
           player->name, player->level, save_path);
    return true;
}

// Helper function to read a line from CSV and extract value
static bool read_csv_value(FILE *file, char *key, char *value, size_t max_len) {
    char line[512];
    
    // Read next line
    if (fgets(line, sizeof(line), file) == NULL) {
        return false;
    }
    
    // Split line by comma
    char *comma = strchr(line, ',');
    if (comma == NULL) {
        return false;
    }
    
    // Extract key
    size_t key_len = comma - line;
    if (key_len >= max_len) {
        key_len = max_len - 1;
    }
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    
    // Extract value
    size_t value_len = strlen(comma + 1);
    if (value_len >= max_len) {
        value_len = max_len - 1;
    }
    strncpy(value, comma + 1, value_len);
    
    // Remove newline character if present
    if (value[value_len - 1] == '\n') {
        value[value_len - 1] = '\0';
    } else {
        value[value_len] = '\0';
    }
    
    return true;
}

// Load player stats from a CSV file
bool load_game(Player *player, const char *filename) {
    if (player == NULL) {
        printf("Error: Can't load to NULL player\n");
        return false;
    }
    
    char save_path[MAX_FILENAME_LENGTH];
    get_save_filename(save_path, player->name, filename);
    
    // Check if save file exists
    if (!save_game_exists(player->name, filename)) {
        printf("No saved game found at '%s'!\n", save_path);
        return false;
    }
    
    FILE *file = fopen(save_path, "r");
    if (file == NULL) {
        printf("Error: Could not open save file '%s'\n", save_path);
        return false;
    }
    
    // Skip header line
    char line[512];
    if (fgets(line, sizeof(line), file) == NULL || strncmp(line, "key,value", 9) != 0) {
        printf("Error: Invalid save file format - missing header\n");
        fclose(file);
        return false;
    }
    
    // Read and parse each line
    char key[100];
    char value[400];
    bool name_found = false;
    
    while (read_csv_value(file, key, value, sizeof(key))) {
        if (strcmp(key, "NAME") == 0) {
            // Free old name if it exists
            if (player->name != NULL) {
                free(player->name);
            }
            
            // Allocate and copy new name
            player->name = malloc(strlen(value) + 1);
            if (player->name == NULL) {
                printf("Error: Failed to allocate memory for player name\n");
                fclose(file);
                return false;
            }
            strcpy(player->name, value);
            name_found = true;
        } 
        else if (strcmp(key, "CLASS") == 0) {
            player->playerClass = atoi(value);
        }
        else if (strcmp(key, "HP") == 0) {
            player->hp = atoi(value);
        }
        else if (strcmp(key, "MAX_HP") == 0) {
            player->maxHp = atoi(value);
        }
        else if (strcmp(key, "DAMAGE") == 0) {
            player->damage = atoi(value);
        }
        else if (strcmp(key, "XP") == 0) {
            player->xp = atoi(value);
        }
        else if (strcmp(key, "LEVEL") == 0) {
            player->level = atoi(value);
        }
        else if (strcmp(key, "KILLS") == 0) {
            player->kills = atoi(value);
        }
        else if (strcmp(key, "GOLD") == 0) {
            player->gold = atoi(value);
        }
        else if (strcmp(key, "AREA") == 0) {
            player->area_level = atoi(value);
        }
        else if (strcmp(key, "IS_POISONED") == 0) {
            player->is_poisoned = (atoi(value) != 0);
        }
        else if (strcmp(key, "IS_SHIELDED") == 0) {
            player->is_shielded = (atoi(value) != 0);
        }
        else if (strcmp(key, "TURN_SKIPPED") == 0) {
            player->turn_skipped = (atoi(value) != 0);
        }
        // Ignore any other keys (like TIMESTAMP)
    }
    
    fclose(file);
    
    if (!name_found) {
        printf("Error: Corrupted save - missing name\n");
        return false;
    }
    
    // Initialize inventory (empty for now)
    player->inventory_capacity = INITIAL_INVENTORY_CAPACITY;
    player->inventory_size = 0;
    player->inventory = malloc(sizeof(Item*) * player->inventory_capacity);
    
    if (player->inventory == NULL) {
        printf("Error: Could not allocate memory for inventory\n");
        return false;
    }
    
    // Initialize all slots to NULL
    for (int i = 0; i < player->inventory_capacity; i++) {
        player->inventory[i] = NULL;
    }
    
    // Give player a health potion when loading a save
    Item *potion = create_health_potion(player->level);
    if (potion != NULL) {
        player->inventory[0] = potion;
        player->inventory_size = 1;
    }
    
    printf("Game loaded successfully for %s (Level %d) from '%s'!\n", 
           player->name, player->level, save_path);
    return true;
}

// Check if a saved game exists
bool save_game_exists(const char *username, const char *filename) {
    char save_path[MAX_FILENAME_LENGTH];
    get_save_filename(save_path, username, filename);
    
    FILE *file = fopen(save_path, "r");
    if (file == NULL) {
        return false;
    }
    
    fclose(file);
    return true;
}

// Clear saved game data
bool clear_save(const char *filename) {
    char save_path[MAX_FILENAME_LENGTH];
    get_save_filename(save_path, NULL, filename);
    
    if (remove(save_path) != 0) {
        printf("Warning: Could not delete save file '%s'\n", save_path);
        return false;
    }
    
    printf("Save data cleared from '%s'!\n", save_path);
    return true;
} 