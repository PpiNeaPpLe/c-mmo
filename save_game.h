// save_game.h - Game save/load functionality
#ifndef SAVE_GAME_H
#define SAVE_GAME_H

#include "player.h"
#include "enemy.h"
#include <stdbool.h>

// Default directory for save files
#define DEFAULT_SAVE_DIR "saves"

// Maximum length for save filename
#define MAX_FILENAME_LENGTH 100

// save player stats to a CSV file
// if filename is NULL, uses {username}.csv
// returns true if save was successful
bool save_game(Player *player, const char *filename);

// load player stats from a CSV file
// if filename is NULL, tries to load {username}.csv
// returns true if load was successful 
bool load_game(Player *player, const char *filename);

// check if a saved game exists
// if filename is NULL, checks for {username}.csv
bool save_game_exists(const char *username, const char *filename);

// clear saved game data
// returns true if clear was successful
bool clear_save(const char *filename);

// Get the full path for the save file
// if filename is NULL, uses {username}.csv
// result must be pre-allocated with at least MAX_FILENAME_LENGTH bytes
void get_save_filename(char *result, const char *username, const char *filename);

#endif // SAVE_GAME_H 