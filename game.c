// game.c - Game logic implementations
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "utils.h" // add utils header
#include <stdio.h>
#include <stdlib.h> // For rand(), srand()
#include <time.h>   // For time()

// Handles the player's turn
void player_turn(Player *player, Enemy *enemy) {
    // bad pointers? get outta here
    if (player == NULL || enemy == NULL) return;

    int choice;
    
    // Show options: Attack, Use Item, Cast Spell (if mage)
    printf("Player %s's turn. Choose action (1: Attack", player->name);
    printf(", 2: Use Item");
    if (player->playerClass == MAGE) {
        printf(", 3: Cast Spell");
    }
    printf("): ");
    
    // see if they typed a number
    if (scanf("%d", &choice) == 1) {
        // eat the enter key press
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        // log the choice with our variadic logging function
        log_event(LOG_DEBUG, "Player chose action %d", choice);

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
                   
            // log combat
            log_event(LOG_COMBAT, "%s dealt %d damage to %s", 
                     player->name, player->damage, enemy->name);
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
                                
                                // log healing
                                log_event(LOG_COMBAT, "%s used %s and healed for %d HP", 
                                         player->name, chosen_item->name, chosen_item->value);
                                
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
        else if (choice == 3 && player->playerClass == MAGE) {
            // --- Cast Spell (new option for mages) ---
            printf("Choose spell to cast:\n");
            printf("  1: Fireball (dmg + burn)\n");
            printf("  2: Frost Nova (dmg + freeze)\n");
            printf("  3: Lightning Bolt (dmg + chain)\n");
            printf("  4: Healing Light (heal over time)\n");
            printf("  5: Random Magic (???) \n");

            int spell_choice;
            printf("Enter spell number (or 0 to cancel): ");
            if (scanf("%d", &spell_choice) == 1) {
                // eat that delicious enter key
                while ((c = getchar()) != '\n' && c != EOF);
                
                if (spell_choice >= 1 && spell_choice <= 5) {
                    int spell_damage = 0;
                    
                    switch (spell_choice) {
                        case 1: { // Fireball
                            // Use our variadic function - pass intensity and burn turns
                            int intensity = rand() % 10 + 1;  // random 1-10
                            int burn_turns = rand() % 3 + 1;  // random 1-3
                            spell_damage = cast_spell(player, enemy, FIRE_SPELL, intensity, burn_turns);
                            break;
                        }
                        case 2: { // Frost Nova
                            // Use our variadic function - pass radius and freeze chance
                            int radius = rand() % 5 + 1;  // random 1-5
                            double freeze_chance = (rand() % 100) / 100.0;  // random 0.0-1.0
                            spell_damage = cast_spell(player, enemy, ICE_SPELL, radius, freeze_chance);
                            break;
                        }
                        case 3: { // Lightning
                            // Use our variadic function - pass power and chain targets
                            int power = rand() % 5 + 1;  // random 1-5
                            int chain_targets = rand() % 5;  // random 0-4
                            spell_damage = cast_spell(player, enemy, LIGHTNING_SPELL, power, chain_targets);
                            break;
                        }
                        case 4: { // Healing
                            // Use our variadic function - pass power and duration
                            int power = rand() % 5 + 1;  // random 1-5
                            int duration = rand() % 3 + 1;  // random 1-3
                            spell_damage = cast_spell(player, enemy, HEAL_SPELL, power, duration);
                            // no damage to enemy for healing spells
                            spell_damage = 0;
                            break;
                        }
                        case 5: { // Random
                            // Use our variadic function - no extra args for random
                            spell_damage = cast_spell(player, enemy, RANDOM_SPELL);
                            break;
                        }
                    }
                    
                    // Apply damage if it's an offensive spell
                    if (spell_damage > 0 && enemy != NULL) {
                        enemy->hp -= spell_damage;
                        if (enemy->hp < 0) enemy->hp = 0;
                        
                        printf("%s takes %d damage from the spell. Remaining HP: %d/%d\n", 
                               enemy->name, spell_damage, enemy->hp, enemy->maxHp);
                               
                        // log spell damage
                        log_event(LOG_COMBAT, "Spell dealt %d damage to %s", 
                                 spell_damage, enemy->name);
                    }
                } else if (spell_choice == 0) {
                    printf("Spell casting cancelled.\n");
                } else {
                    printf("Invalid spell choice.\n");
                }
            } else {
                // they typed garbage
                printf("Invalid input. Please enter a spell number.\n");
                while ((c = getchar()) != '\n' && c != EOF);
            }
        }
        else {
            // wrong action choice (not valid)
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
           
    // log enemy damage with our variadic function
    log_event(LOG_COMBAT, "%s dealt %d damage to %s", 
             enemy->name, enemy->damage, player->name);
} 