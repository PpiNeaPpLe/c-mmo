#include "save_game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// environment variable prefix for save data
#define SAVE_PREFIX "CMMO_SAVE_"

// save player stats to environment vars
bool save_game(Player *player) {
    if (player == NULL) {
        printf("Error: Can't save NULL player\n");
        return false;
    }
    
    char env_var[100]; // buffer for env var names
    char value[100];   // buffer for env var values
    
    // save basic player stats
    sprintf(env_var, "%sNAME", SAVE_PREFIX);
    setenv(env_var, player->name, 1);
    
    sprintf(env_var, "%sCLASS", SAVE_PREFIX);
    sprintf(value, "%d", player->playerClass);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sHP", SAVE_PREFIX);
    sprintf(value, "%d", player->hp);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sMAX_HP", SAVE_PREFIX);
    sprintf(value, "%d", player->maxHp);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sDAMAGE", SAVE_PREFIX);
    sprintf(value, "%d", player->damage);
    setenv(env_var, value, 1);
    
    // save progression stats
    sprintf(env_var, "%sXP", SAVE_PREFIX);
    sprintf(value, "%d", player->xp);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sLEVEL", SAVE_PREFIX);
    sprintf(value, "%d", player->level);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sKILLS", SAVE_PREFIX);
    sprintf(value, "%d", player->kills);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sGOLD", SAVE_PREFIX);
    sprintf(value, "%d", player->gold);
    setenv(env_var, value, 1);
    
    sprintf(env_var, "%sAREA", SAVE_PREFIX);
    sprintf(value, "%d", player->area_level);
    setenv(env_var, value, 1);
    
    // save status flags
    sprintf(env_var, "%sSTATUS", SAVE_PREFIX);
    sprintf(value, "%d%d%d", 
            player->is_poisoned,
            player->is_shielded,
            player->turn_skipped);
    setenv(env_var, value, 1);
    
    // save timestamp to mark when game was saved
    sprintf(env_var, "%sTIMESTAMP", SAVE_PREFIX);
    sprintf(value, "%ld", (long)time(NULL));
    setenv(env_var, value, 1);
    
    // save a flag to mark that save exists
    sprintf(env_var, "%sEXISTS", SAVE_PREFIX);
    setenv(env_var, "1", 1);
    
    printf("Game saved successfully for %s (Level %d)!\n", 
           player->name, player->level);
    return true;
}

// load player stats from environment vars
bool load_game(Player *player) {
    if (player == NULL) {
        printf("Error: Can't load to NULL player\n");
        return false;
    }
    
    // check if save exists first
    if (!save_game_exists()) {
        printf("No saved game found!\n");
        return false;
    }
    
    char env_var[100];  // buffer for env var names
    const char *value;  // pointer for env var values
    
    // --- Load player name (need to allocate memory) ---
    sprintf(env_var, "%sNAME", SAVE_PREFIX);
    value = getenv(env_var);
    if (value == NULL) {
        printf("Error: Corrupted save - missing name\n");
        return false;
    }
    
    // free old name if it exists
    if (player->name != NULL) {
        free(player->name);
    }
    
    // allocate and copy new name
    player->name = malloc(strlen(value) + 1);
    if (player->name == NULL) {
        printf("Error: Failed to allocate memory for player name\n");
        return false;
    }
    strcpy(player->name, value);
    
    // --- Load player class ---
    sprintf(env_var, "%sCLASS", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->playerClass = atoi(value);
    } else {
        printf("Warning: Missing class in save\n");
    }
    
    // --- Load stats ---
    sprintf(env_var, "%sHP", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->hp = atoi(value);
    }
    
    sprintf(env_var, "%sMAX_HP", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->maxHp = atoi(value);
    }
    
    sprintf(env_var, "%sDAMAGE", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->damage = atoi(value);
    }
    
    // --- Load progression stats ---
    sprintf(env_var, "%sXP", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->xp = atoi(value);
    }
    
    sprintf(env_var, "%sLEVEL", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->level = atoi(value);
    }
    
    sprintf(env_var, "%sKILLS", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->kills = atoi(value);
    }
    
    sprintf(env_var, "%sGOLD", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->gold = atoi(value);
    }
    
    sprintf(env_var, "%sAREA", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL) {
        player->area_level = atoi(value);
    }
    
    // --- Load status flags ---
    sprintf(env_var, "%sSTATUS", SAVE_PREFIX);
    value = getenv(env_var);
    if (value != NULL && strlen(value) >= 3) {
        player->is_poisoned = value[0] == '1';
        player->is_shielded = value[1] == '1';
        player->turn_skipped = value[2] == '1';
    }
    
    // --- Initialize inventory (empty for now) ---
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
    
    // give player a health potion when loading a save
    Item *potion = create_health_potion(player->level);
    if (potion != NULL) {
        player->inventory[0] = potion;
        player->inventory_size = 1;
    }
    
    printf("Game loaded successfully for %s (Level %d)!\n", 
           player->name, player->level);
    return true;
}

// check if a saved game exists
bool save_game_exists() {
    char env_var[100];
    sprintf(env_var, "%sEXISTS", SAVE_PREFIX);
    const char *exists = getenv(env_var);
    
    return (exists != NULL && strcmp(exists, "1") == 0);
}

// clear saved game data
bool clear_save() {
    // just an array of all the env vars we set
    const char *env_vars[] = {
        "NAME", "CLASS", "HP", "MAX_HP", "DAMAGE",
        "XP", "LEVEL", "KILLS", "GOLD", "AREA",
        "STATUS", "TIMESTAMP", "EXISTS", NULL  // NULL terminator
    };
    
    char env_var[100];
    int i = 0;
    
    // loop through and unset each var
    while (env_vars[i] != NULL) {
        sprintf(env_var, "%s%s", SAVE_PREFIX, env_vars[i]);
        unsetenv(env_var);
        i++;
    }
    
    printf("Save data cleared!\n");
    return true;
} 