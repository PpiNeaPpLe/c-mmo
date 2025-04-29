#include "save_game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

// Maximum number of items that can be saved/loaded
#define MAX_INVENTORY_CAPACITY 20

#ifdef _WIN32
#include <direct.h>  // For _mkdir on Windows
#define mkdir(dir, mode) _mkdir(dir)  // Windows doesn't use mode
#endif

// Helper function to ensure the save directory exists
static bool ensure_save_directory() {
    // Create save directory if it doesn't exist
    if (mkdir(DEFAULT_SAVE_DIR, 0755) != 0 && errno != EEXIST) {
        printf("Warning: Could not create save directory\n");
        return false;
    }
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
    
    // Write inventory data
    fprintf(file, "INV_SIZE,%d\n", player->inventory_size);
    fprintf(file, "INV_CAPACITY,%d\n", player->inventory_capacity);
    
    // Save each inventory item
    for (int i = 0; i < player->inventory_size; i++) {
        Item *item = player->inventory[i];
        if (item != NULL) {
            fprintf(file, "ITEM_%d_TYPE,%d\n", i, item->type);
            fprintf(file, "ITEM_%d_NAME,%s\n", i, item->name);
            fprintf(file, "ITEM_%d_STRENGTH,%d\n", i, item->strength);
        }
    }
    
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
    
    // The key fix: using the global saveFileName if provided, otherwise check for player->name.csv
    // But we should only use player->name if it's not the temporary name "temp"
    const char* username = (player->name != NULL && strcmp(player->name, "temp") != 0) ? player->name : NULL;
    
    // Determine the path to the save file based on provided filename or username
    get_save_filename(save_path, username, filename);
    
    // Check if save file exists
    if (!save_game_exists(username, filename)) {
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
    int saved_inventory_size = 0;
    int saved_inventory_capacity = INITIAL_INVENTORY_CAPACITY;
    
    // Arrays to temporarily store item data while parsing
    int item_types[MAX_INVENTORY_CAPACITY];
    char item_names[MAX_INVENTORY_CAPACITY][64];
    int item_strengths[MAX_INVENTORY_CAPACITY];
    bool item_exists[MAX_INVENTORY_CAPACITY] = {false};
    
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
        else if (strcmp(key, "INV_SIZE") == 0) {
            saved_inventory_size = atoi(value);
        }
        else if (strcmp(key, "INV_CAPACITY") == 0) {
            saved_inventory_capacity = atoi(value);
        }
        // Check for item data - parse keys like ITEM_0_TYPE, ITEM_0_NAME, etc.
        else if (strncmp(key, "ITEM_", 5) == 0) {
            // Extract item index from the key (e.g., from "ITEM_0_TYPE" get 0)
            int item_index = atoi(key + 5);
            if (item_index >= 0 && item_index < MAX_INVENTORY_CAPACITY) {
                // Check which property this is (TYPE, NAME, STRENGTH)
                if (strstr(key, "_TYPE") != NULL) {
                    item_types[item_index] = atoi(value);
                    item_exists[item_index] = true;
                }
                else if (strstr(key, "_NAME") != NULL) {
                    strncpy(item_names[item_index], value, 63);
                    item_names[item_index][63] = '\0'; // Ensure null-termination
                }
                else if (strstr(key, "_STRENGTH") != NULL) {
                    item_strengths[item_index] = atoi(value);
                }
            }
        }
        // Ignore any other keys (like TIMESTAMP)
    }
    
    fclose(file);
    
    if (!name_found) {
        printf("Error: Corrupted save - missing name\n");
        return false;
    }
    
    // Initialize inventory
    player->inventory_capacity = saved_inventory_capacity;
    player->inventory_size = 0; // Will be incremented as we add items
    player->inventory = malloc(sizeof(Item*) * player->inventory_capacity);
    
    if (player->inventory == NULL) {
        printf("Error: Could not allocate memory for inventory\n");
        return false;
    }
    
    // Initialize all slots to NULL
    for (int i = 0; i < player->inventory_capacity; i++) {
        player->inventory[i] = NULL;
    }
    
    // Create items from saved data
    for (int i = 0; i < saved_inventory_size; i++) {
        if (item_exists[i]) {
            // Allocate memory for the item
            Item *item = malloc(sizeof(Item));
            if (item != NULL) {
                item->type = item_types[i];
                strncpy(item->name, item_names[i], 63);
                item->name[63] = '\0'; // Ensure null-termination
                item->strength = item_strengths[i];
                
                // Add to inventory
                player->inventory[player->inventory_size] = item;
                player->inventory_size++;
            }
        }
    }
    
    // If inventory is empty (possibly due to error), add a health potion
    if (player->inventory_size == 0) {
        Item *potion = create_health_potion(player->level);
        if (potion != NULL) {
            player->inventory[0] = potion;
            player->inventory_size = 1;
        }
    }
    
    printf("Game loaded successfully for %s (Level %d) from '%s'!\n", 
           player->name, player->level, save_path);
    return true;
}

// Check if a saved game exists
bool save_game_exists(const char *username, const char *filename) {
    char save_path[MAX_FILENAME_LENGTH];
    
    // If we have a filename, it takes precedence
    if (filename != NULL && filename[0] != '\0') {
        get_save_filename(save_path, NULL, filename);
    } 
    // Otherwise use username
    else if (username != NULL && username[0] != '\0') {
        get_save_filename(save_path, username, NULL);
    }
    // Fallback to default if neither exists
    else {
        get_save_filename(save_path, NULL, "default.csv");
    }
    
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