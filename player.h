// player.h - Header for player structure and functions
#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h> // Include for bool type
#include "items.h" // need this for Item type

#define MAX_NAME_LENGTH 50
#define INITIAL_INVENTORY_CAPACITY 5 // how many items can we hold at start

// define the classes
enum ClassType {
    PALADIN, // 0
    ROGUE,   // 1
    MAGE     // 2
    // maybe add more later?
};

// Basic Player structure
typedef struct {
    char *name;
    int hp;
    int maxHp;
    int damage;
    enum ClassType playerClass; // what class r they
    
    // new stuff for saving & progression!!
    int xp;          // how much XP we have
    int level;       // what level r we at
    int kills;       // how many monsters we've killed
    int gold;        // money for buying stuff maybe?
    int area_level;  // which area we're in

    // inventory stuff
    Item **inventory;       // array of pointers to items, needs malloc
    int inventory_size;     // how many items we HAVE
    int inventory_capacity; // how many slots we allocated

    //  bitfields for statuses
    unsigned int is_poisoned : 1;
    unsigned int is_shielded : 1;
    unsigned int turn_skipped : 1;

} Player;

// Function prototypes for player actions will go here
// For example:
// void initialize_player(Player *player);
void initialize_player(Player *player, const char *name, bool god_mode);
void cleanup_player(Player *player); // need func to free inventory later

// add xp to player & level up if needed
// returns true if leveled up
bool add_player_xp(Player *player, int xp_amount);

#endif // PLAYER_H 