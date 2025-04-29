// player.c - Player related functions
#include "player.h"
#include "items.h" // need this too
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Need for malloc, free, exit
#include <ctype.h> // for isdigit maybe? nah lets just use scanf
#include <stdbool.h> // include bool for god_mode flag

// Clears the input buffer after scanf fails or reads part of a line
void clear_input_buffer() {
    int c;
    // read chars until newline or end-of-file
    while ((c = getchar()) != '\n' && c != EOF);
}

// Initialize the player with a name and prompt for class
void initialize_player(Player *player, const char *name, bool god_mode) {
    // check for dumb stuff
    if (player == NULL || name == NULL) {
        fprintf(stderr, "Error: Cannot initialize player with NULL pointers.\n");
        return; 
    }

    // copy the name over
    player->name = malloc(strlen(name) + 1); // +1 for null terminator
    if (player->name == NULL) {
        fprintf(stderr, "Fatal Error: Could not allocate memory for player name.\n");
        // no inventory to free yet
        exit(1); 
    }
    // Now copy the name using strcpy (safe because we allocated enough space)
    strcpy(player->name, name); 
    
    // Initialize status flags to prevent uninitialized values
    player->is_poisoned = 0;
    player->is_shielded = 0;
    player->turn_skipped = 0;
    
    int choice = 0;
    int validInput = 0;

    printf("\nAlright %s, pick your class:\n", player->name); // tell em to pick
    printf("  1. Paladin (Tough, decent damage)\n");
    printf("  2. Rogue   (Squishy, high damage)\n");
    printf("  3. Mage    (Average, does magic stuff later maybe)\n");

    // keep asking till they give a good number
    while (!validInput) {
        printf("Enter choice (1-3): ");
        // try read number
        if (scanf("%d", &choice) == 1) {
            clear_input_buffer(); // clear rest of line like the enter key
            
            // see if number is good
            if (choice >= 1 && choice <= 3) {
                validInput = 1; // yay they did it
            }
            else {
                printf("Dude, enter 1, 2, or 3.\n"); // tell em off
            }
        }
        else {
            printf("That's not even a number. Try again.\n"); // really tell em off
            clear_input_buffer(); // clean up the mess they made in input
        }
    }

    // set stats based on choice
    // remember array index starts at 0, choices are 1, 2, 3
    switch (choice) {
        case 1: // Paladin
            player->playerClass = PALADIN;
            player->hp = 60;    // more hp
            player->damage = 7; // less dmg
            printf("You are a Paladin! Holy light and stuff.\n");
            break;
        case 2: // Rogue
            player->playerClass = ROGUE;
            player->hp = 40;    // less hp
            player->damage = 10; // more dmg
            printf("You are a Rogue! Sneaky sneaky.\n");
            break;
        case 3: // Mage
            player->playerClass = MAGE;
            player->hp = 45;    // medium hp
            player->damage = 8; // medium dmg
            printf("You are a Mage! Zap zap.\n");
            break;
        // dont need default because we looped until valid input
    }

    player->maxHp = player->hp; // max hp is same as starting hp for now

    // init the new stats at level 1
    player->xp = 0;
    player->level = 1;
    player->kills = 0;
    player->gold = 10; // start with a bit of gold
    player->area_level = 1; // starting area
    
    // --- Apply God Mode Stats --- 
    if (god_mode) {
        printf("*** GOD MODE STATS APPLIED ***\n");
        player->hp = 9999;
        player->maxHp = 9999;
        player->damage = 999;
        player->gold = 9999; // lots of money in god mode lol
    }

    // ---- Initialize Inventory ----
    player->inventory_capacity = INITIAL_INVENTORY_CAPACITY;
    player->inventory_size = 0;
    // Allocate memory for the array of Item pointers
    player->inventory = malloc(sizeof(Item*) * player->inventory_capacity);
    if (player->inventory == NULL) {
        // major problem, cant allocate memory
        fprintf(stderr, "Fatal Error: Could not allocate memory for inventory.\n");
        exit(1); // exit the whole game, cant continue
    }

    // make all slots empty initially
    for (int i = 0; i < player->inventory_capacity; ++i) {
        player->inventory[i] = NULL;
    }

    // --- Give Starting Items Based on Class ---
    Item *starting_item = NULL;
    
    // Create different starting items based on class
    if (player->playerClass == PALADIN) {
        // Paladins get a stronger basic potion
        starting_item = create_health_potion(3); // strength 3
    } else if (player->playerClass == ROGUE) {
        // Rogues get a random potion (they stole it)
        starting_item = create_random_potion();
    } else {
        // Mages just get a basic potion (they have spells)
        starting_item = create_health_potion(2); // strength 2
    }
    
    // Safety check for failed item creation
    if (starting_item == NULL) {
        // Fallback to basic potion if creation failed
        starting_item = create_item(HEALING, "Basic Health Potion", 20);
        
        if (starting_item == NULL) {
            // Really bad - can't create any items
            fprintf(stderr, "Fatal Error: Could not create starting item.\n");
            free(player->inventory);
            exit(1);
        }
    }

    // Add the potion to the first inventory slot
    if (player->inventory_size < player->inventory_capacity) { 
        player->inventory[player->inventory_size] = starting_item;
        player->inventory_size++;
        printf("Added %s to inventory.\n", starting_item->name);
    } else {
        // this shouldnt happen right at the start but good check maybe
        printf("Inventory full? Couldn't add starting potion.\n");
        // BIG PROBLEM: we allocated memory for the potion but can't store it
        // so we MUST free it here to avoid a memory leak
        free(starting_item->name);
        free(starting_item);
    }

    printf("Player %s (%s) created! HP: %d/%d, Damage: %d, Level: %d\n", 
           player->name, 
           (player->playerClass == PALADIN) ? "Paladin" : (player->playerClass == ROGUE) ? "Rogue" : "Mage", // show class name
           player->hp, player->maxHp, player->damage, player->level);
}

// free memory allocated for player stuff
void cleanup_player(Player *player) {
    if (player == NULL) return;

    // --- FIX: Free player name if allocated ---
    if (player->name != NULL) {
        printf("Cleaning up player %s...\n", player->name); // Print name before freeing
        free(player->name);
        player->name = NULL;
    } else {
        printf("Cleaning up unnamed player...\n");
    }

    // free each item AND its name string
    if (player->inventory != NULL) {
        for (int i = 0; i < player->inventory_size; ++i) { // only free items we actually have
            if (player->inventory[i] != NULL) {
                printf("  Freeing item: %s\n", player->inventory[i]->name);
                if (player->inventory[i]->name != NULL) {
                    free(player->inventory[i]->name); // free name string first
                }
                free(player->inventory[i]); // then free item struct
                player->inventory[i] = NULL; // prevent double free maybe?
            }
        }
        // now free the inventory array itself
        printf("  Freeing inventory array.\n");
        free(player->inventory);
        player->inventory = NULL;
    }
    // --- Remove redundant cleanup messages ---
    // player->inventory_size = 0; // No need to reset these after freeing
    // player->inventory_capacity = 0;
}

// Add XP to player and level up if needed
bool add_player_xp(Player *player, int xp_amount) {
    if (player == NULL || xp_amount <= 0) {
        return false; // invalid inputs
    }
    
    // calculate how much XP needed for next level
    // simple formula: 100 * current_level
    int xp_needed = 100 * player->level;
    
    // add the XP
    player->xp += xp_amount;
    printf("%s gained %d XP! (Total: %d/%d)\n", 
           player->name, xp_amount, player->xp, xp_needed);
    
    // check if we leveled up
    if (player->xp >= xp_needed) {
        // level up!!
        player->level++;
        player->xp -= xp_needed; // carry over extra XP
        
        // improve stats based on class
        switch (player->playerClass) {
            case PALADIN:
                player->maxHp += 10;  // more HP for tanks
                player->damage += 1;  // small damage boost
                break;
                
            case ROGUE:
                player->maxHp += 5;   // small HP boost
                player->damage += 3;  // bigger damage boost
                break;
                
            case MAGE:
                player->maxHp += 7;   // medium HP boost
                player->damage += 2;  // medium damage boost
                break;
        }
        
        // heal to full on level up cuz im nice lol
        player->hp = player->maxHp;
        
        printf("\n🎉 LEVEL UP! 🎉\n");
        printf("%s is now level %d!\n", player->name, player->level);
        printf("Max HP increased to %d\n", player->maxHp);
        printf("Damage increased to %d\n", player->damage);
        printf("Health restored to full!\n");
        
        return true; // we did level up
    }
    
    return false; // didn't level up
} 