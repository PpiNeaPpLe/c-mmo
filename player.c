// player.c - Player related functions
#include "player.h"
#include "items.h" // need this too
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Need for malloc, free, exit
#include <ctype.h> // for isdigit maybe? nah lets just use scanf

// Clears the input buffer after scanf fails or reads part of a line
void clear_input_buffer() {
    int c;
    // read chars until newline or end-of-file
    while ((c = getchar()) != '\n' && c != EOF);
}

// Initialize the player with a name and prompt for class
void initialize_player(Player *player, const char *name) {
    // check for dumb stuff
    if (player == NULL || name == NULL) {
        fprintf(stderr, "Error: Cannot initialize player with NULL pointers.\n");
        return; 
    }

    // copy the name over
    strncpy(player->name, name, MAX_NAME_LENGTH - 1);
    player->name[MAX_NAME_LENGTH - 1] = '\0'; // make sure it ends right
    
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

    // --- Give Starting Item: Health Potion ---
    // Allocate memory for the Item struct itself
    Item *health_potion = malloc(sizeof(Item));
    if (health_potion == NULL) {
        fprintf(stderr, "Fatal Error: Could not allocate memory for health potion item.\n");
        // Need to free inventory array before exiting
        free(player->inventory);
        exit(1);
    }

    // Allocate memory for the item name string
    // Use a fixed size buffer temporarily to calculate length
    char potion_name[] = "Health Potion";
    health_potion->name = malloc(strlen(potion_name) + 1); // +1 for null terminator
    if (health_potion->name == NULL) {
        fprintf(stderr, "Fatal Error: Could not allocate memory for potion name.\n");
        // Need to free item struct and inventory array before exiting
        free(health_potion);
        free(player->inventory);
        exit(1);
    }
    strcpy(health_potion->name, potion_name); // copy name in

    // set other potion fields
    health_potion->type = HEALING;
    health_potion->value = 20; // heals 20 hp maybe

    // Add potion to the first inventory slot
    if (player->inventory_size < player->inventory_capacity) { 
        player->inventory[player->inventory_size] = health_potion;
        player->inventory_size++;
        printf("Added %s to inventory.\n", health_potion->name);
    } else {
        // this shouldnt happen right at the start but good check maybe
        printf("Inventory full? Couldn't add starting potion.\n");
        // BIG PROBLEM: we allocated memory for the potion but can't store it
        // so we MUST free it here to avoid a memory leak
        free(health_potion->name);
        free(health_potion);
    }

    printf("Player %s (%s) created! HP: %d/%d, Damage: %d\n", 
           player->name, 
           (player->playerClass == PALADIN) ? "Paladin" : (player->playerClass == ROGUE) ? "Rogue" : "Mage", // show class name
           player->hp, player->maxHp, player->damage);
}

// free memory allocated for player stuff
void cleanup_player(Player *player) {
    if (player == NULL) return;

    printf("Cleaning up player %s...\n", player->name);

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
    player->inventory_size = 0;
    player->inventory_capacity = 0;
} 