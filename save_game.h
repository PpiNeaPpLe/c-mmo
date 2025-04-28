// save_game.h - Game save/load functionality
#ifndef SAVE_GAME_H
#define SAVE_GAME_H

#include "player.h"
#include "enemy.h"
#include <stdbool.h>

// save player stats to environment vars
// returns true if save was successful
bool save_game(Player *player);

// load player stats from environment vars
// returns true if load was successful 
bool load_game(Player *player);

// check if a saved game exists
bool save_game_exists();

// clear saved game data
// returns true if clear was successful
bool clear_save();

#endif // SAVE_GAME_H 