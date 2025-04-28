// game.h - Header for game logic
#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"

// Game state enum
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_COMBAT,
    GAME_STATE_EXPLORE,
    GAME_STATE_SHOP,
    GAME_STATE_GAME_OVER,
    GAME_STATE_WIN
} GameState;

// Function prototypes for game logic will go here
// For example:
// void start_combat(Player *player, Enemy *enemy);

// Handles the player's turn
void player_turn(Player *player, Enemy *enemy);

// Handles the enemy's turn
void enemy_turn(Player *player, Enemy *enemy);

// Start combat with an enemy
void start_combat(Player *player, Enemy *enemy);

// Handle enemy death rewards
void handle_enemy_defeat(Player *player, Enemy *enemy);

// Show shop menu and handle purchases
void show_shop(Player *player);

// Show exploration menu options
void explore_area(Player *player);

// Main game loop
void game_loop(Player *player, GameState *state);

#endif // GAME_H 