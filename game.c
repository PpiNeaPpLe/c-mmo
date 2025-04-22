// game.c - Game logic implementations
#include "game.h"
#include "player.h"
#include "enemy.h"
#include <stdio.h>
#include <stdlib.h> // For rand(), srand()
#include <time.h>   // For time()

// Handles the player's turn
void player_turn(Player *player, Enemy *enemy) {
    // bad pointers? get outta here
    if (player == NULL || enemy == NULL) return;

    // Show options: Attack or Use Item
    printf("Player %s's turn. Choose action (1: Attack, 2: Use Item): ", player->name);
    int choice;
    
    // see if they typed a number
    if (scanf("%d", &choice) == 1) {
        // eat the enter key press
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (choice == 1) {
            // ok they chose attack
            
            // figure out what class they are for the message
            const char* attackVerb = "attacks"; // default
            switch(player->playerClass) {
                case PALADIN:
                    attackVerb = "swings their hammer at"; // pally attack
                    break;
                case ROGUE:
                    attackVerb = "stabs sneakily at"; // rogue attack
                    break;
                case MAGE:
                    attackVerb = "flings a weak spark at"; // mage attack
                    break;
                // no default needed bc playerClass should always be one of these
            }

            printf("%s %s %s!\n", player->name, attackVerb, enemy->name);
            
            // do the damage
            enemy->hp -= player->damage; 
            
            // make sure hp isnt weirdly negative
            if (enemy->hp < 0) enemy->hp = 0;

            printf("%s takes %d damage. Remaining HP: %d/%d\n", 
                   enemy->name, player->damage, enemy->hp, enemy->maxHp);
        }
        else if (choice == 2) {
            // --- Use Item Logic ---
            printf("Choose item to use:\n");
            if (player->inventory_size == 0) {
                printf("  Inventory empty!\n");
                // maybe re-prompt? nah just skip turn for now
            }
            else {
                // list items (only potion for now)
                for (int i = 0; i < player->inventory_size; ++i) { 
                    if (player->inventory[i] != NULL) { // check if slot not empty
                        printf("  %d: %s\n", i + 1, player->inventory[i]->name);
                    }
                }
                printf("Enter item number (or 0 to cancel): ");
                int item_choice;
                if (scanf("%d", &item_choice) == 1) {
                    // eat the enter key
                    while ((c = getchar()) != '\n' && c != EOF);

                    if (item_choice > 0 && item_choice <= player->inventory_size) {
                        // valid item chosen (adjust index)
                        int item_index = item_choice - 1;
                        Item *chosen_item = player->inventory[item_index];

                        if (chosen_item != NULL) {
                            // --- Use the item --- 
                            printf("Using %s...\n", chosen_item->name);
                            if (chosen_item->type == HEALING) {
                                player->hp += chosen_item->value;
                                // dont overheal
                                if (player->hp > player->maxHp) {
                                    player->hp = player->maxHp;
                                }
                                printf("%s healed! Current HP: %d/%d\n", 
                                       player->name, player->hp, player->maxHp);
                                
                                // --- IMPORTANT: Free memory and remove item ---
                                printf("Freeing used item: %s\n", chosen_item->name);
                                free(chosen_item->name); // free the name string
                                free(chosen_item);       // free the item struct
                                
                                // Remove from inventory (simple version: just shift items down)
                                // This is inefficient but easy for a small inventory
                                for (int j = item_index; j < player->inventory_size - 1; ++j) {
                                    player->inventory[j] = player->inventory[j+1];
                                }
                                player->inventory[player->inventory_size - 1] = NULL; // clear last slot
                                player->inventory_size--;

                            } else {
                                printf("Don't know how to use this item type yet.\n");
                            }
                        } else {
                            printf("Invalid item slot?\n");
                        }
                    } else if (item_choice == 0) {
                         printf("Cancelled using item.\n");
                    } else {
                        printf("Invalid item number.\n");
                    }
                }
                else {
                    // bad input for item choice
                    printf("Invalid input. Please enter an item number.\n");
                    while ((c = getchar()) != '\n' && c != EOF);
                }
            }
        }
        else {
            // wrong action choice (not 1 or 2)
            printf("Invalid action choice. Turn skipped.\n");
        }
    }
    else {
        // they typed letters or something for action choice
        printf("Invalid input. Please enter a number. Turn skipped.\n");
        // clear the bad input
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

// Handles the enemy's turn (simple attack)
void enemy_turn(Player *player, Enemy *enemy) {
    // cant attack if enemy is dead or pointers are bad
    if (player == NULL || enemy == NULL || enemy->hp <= 0) return;

    printf("\n%s's turn.\n", enemy->name); // enemy turn
    printf("%s attacks %s!\n", enemy->name, player->name); // enemy attack
    
    // player takes damage
    player->hp -= enemy->damage; 

    // dont let hp go below 0
    if (player->hp < 0) player->hp = 0;

    printf("%s takes %d damage. Remaining HP: %d/%d\n", 
           player->name, enemy->damage, player->hp, player->maxHp);
} 