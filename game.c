// game.c - Game logic implementations
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "utils.h" // add utils header
#include "save_game.h" // add save game header
#include <stdio.h>
#include <stdlib.h> // For rand(), srand()
#include <time.h>   // For time()
#include <ctype.h>  // For tolower()
#include <string.h> // For strcmp()

// Global variable for total turns taken in current combat
static int g_combat_turn_count = 0;

// External reference to global save filename
extern char saveFileName[MAX_FILENAME_LENGTH];

// Helper to get user confirmation
bool get_yes_no(const char *prompt) {
    char input[10];
    printf("%s (y/n): ", prompt);
    if (scanf("%9s", input) != 1) {
        // clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }
    
    // clear the rest of the line
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // convert input to lowercase
    for (int i = 0; input[i]; i++) {
        input[i] = tolower(input[i]);
    }
    
    // check if starts with 'y'
    return (input[0] == 'y');
}

// start combat with an enemy
void start_combat(Player *player, Enemy *enemy) {
    if (player == NULL || enemy == NULL) {
        return;
    }
    
    // reset combat turn counter
    g_combat_turn_count = 0;
    
    printf("\n--- COMBAT START ---\n");
    printf("You face a Level %d %s!\n", enemy->level, enemy->name);
    
    // Combat loop
    while (player->hp > 0 && enemy->hp > 0) {
        g_combat_turn_count++;
        printf("\n--- Turn %d ---\n", g_combat_turn_count);
        
        // Player goes first
        player_turn(player, enemy);
        
        // Check if enemy is defeated
        if (enemy->hp <= 0) {
            printf("\n%s has been defeated!\n", enemy->name);
            handle_enemy_defeat(player, enemy);
            break;
        }
        
        // Enemy's turn
        enemy_turn(player, enemy);
        
        // Check if player is defeated
        if (player->hp <= 0) {
            printf("\nYou have been defeated by %s!\n", enemy->name);
            printf("GAME OVER\n");
            break;
        }
    }
}

// Handle rewards when enemy is defeated
void handle_enemy_defeat(Player *player, Enemy *enemy) {
    if (player == NULL || enemy == NULL) {
        return;
    }
    
    printf("\n--- VICTORY! ---\n");
    
    // Add gold
    player->gold += enemy->gold_value;
    printf("You found %d gold! (Total: %d)\n", enemy->gold_value, player->gold);
    
    // Add XP and check for level up - actually use the return value
    printf("Gained %d experience!\n", enemy->xp_value);
    bool leveled_up = add_player_xp(player, enemy->xp_value);
    
    // If level up, give a bonus
    if (leveled_up) {
        printf("Bonus for leveling up: +10 gold!\n");
        player->gold += 10;
    }
    
    // Increment kill counter
    player->kills++;
    
    // Random item drop (30% chance)
    if (rand() % 100 < 30 && player->inventory_size < player->inventory_capacity) {
        Item *dropped_item = NULL;
        
        // Better enemies drop better items
        if (enemy->type >= TROLL) {
            dropped_item = create_health_potion(player->level + 1);
        } else {
            dropped_item = create_health_potion(player->level);
        }
        
        if (dropped_item != NULL) {
            player->inventory[player->inventory_size] = dropped_item;
            player->inventory_size++;
            printf("Enemy dropped %s! Added to inventory.\n", dropped_item->name);
        }
    }
    
    // Autosave after battle
    save_game(player, saveFileName[0] != '\0' ? saveFileName : NULL);
}

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
            printf("  5: Random Magic (random effect) \n");

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

// Show shop menu and handle purchases
void show_shop(Player *player) {
    if (player == NULL) {
        return;
    }
    
    printf("\n=== SHOP ===\n");
    printf("Your Gold: %d\n", player->gold);
    printf("1. Health Potion (20 gold)\n");
    printf("2. Strong Health Potion (40 gold)\n");
    printf("3. Super Health Potion (80 gold)\n");
    printf("4. Exit Shop\n");
    
    int choice;
    printf("What would you like to buy? ");
    
    if (scanf("%d", &choice) != 1) {
        // clear invalid input
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Invalid input. Leaving shop.\n");
        return;
    }
    
    // clear newline
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // process choice
    switch (choice) {
        case 1: // Basic health potion
            if (player->gold >= 20) {
                if (player->inventory_size < player->inventory_capacity) {
                    Item *potion = create_health_potion(player->level);
                    player->inventory[player->inventory_size++] = potion;
                    player->gold -= 20;
                    printf("Purchased %s for 20 gold. Remaining gold: %d\n", 
                           potion->name, player->gold);
                } else {
                    printf("Inventory full! Can't buy more items.\n");
                }
            } else {
                printf("Not enough gold!\n");
            }
            break;
            
        case 2: // Stronger health potion
            if (player->gold >= 40) {
                if (player->inventory_size < player->inventory_capacity) {
                    Item *potion = create_health_potion(player->level + 1);
                    player->inventory[player->inventory_size++] = potion;
                    player->gold -= 40;
                    printf("Purchased %s for 40 gold. Remaining gold: %d\n", 
                           potion->name, player->gold);
                } else {
                    printf("Inventory full! Can't buy more items.\n");
                }
            } else {
                printf("Not enough gold!\n");
            }
            break;
            
        case 3: // Super health potion
            if (player->gold >= 80) {
                if (player->inventory_size < player->inventory_capacity) {
                    Item *potion = create_health_potion(player->level + 2);
                    player->inventory[player->inventory_size++] = potion;
                    player->gold -= 80;
                    printf("Purchased %s for 80 gold. Remaining gold: %d\n", 
                           potion->name, player->gold);
                } else {
                    printf("Inventory full! Can't buy more items.\n");
                }
            } else {
                printf("Not enough gold!\n");
            }
            break;
            
        case 4: // Exit
            printf("Thanks for visiting the shop!\n");
            break;
            
        default:
            printf("Invalid choice. Leaving shop.\n");
    }
    
    // auto-save after shopping
    save_game(player, saveFileName[0] != '\0' ? saveFileName : NULL);
}

// Show exploration menu options
void explore_area(Player *player) {
    if (player == NULL) {
        return;
    }
    
    printf("\n=== AREA %d EXPLORATION ===\n", player->area_level);
    printf("1. Fight monster\n");
    printf("2. Rest (heal %d HP)\n", player->level * 5);
    printf("3. Move to next area\n");
    printf("4. Return to menu\n");
    
    int choice;
    printf("What would you like to do? ");
    
    if (scanf("%d", &choice) != 1) {
        // clear invalid input
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Invalid input.\n");
        return;
    }
    
    // clear newline
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    switch (choice) {
        case 1: { // Fight monster
            Enemy enemy;
            initialize_enemy(&enemy, player->area_level, player->level);
            start_combat(player, &enemy);
            cleanup_enemy(&enemy);
            break;
        }
        
        case 2: { // Rest to heal
            int heal_amount = player->level * 5;
            player->hp += heal_amount;
            if (player->hp > player->maxHp) {
                player->hp = player->maxHp;
            }
            printf("You rest and recover %d HP. Current HP: %d/%d\n", 
                   heal_amount, player->hp, player->maxHp);
            save_game(player, saveFileName[0] != '\0' ? saveFileName : NULL);
            break;
        }
        
        case 3: { // Move to next area
            if (player->area_level < 5) {
                if (player->level >= player->area_level + 1) {
                    player->area_level++;
                    printf("You advance to Area %d!\n", player->area_level);
                    save_game(player, saveFileName[0] != '\0' ? saveFileName : NULL);
                } else {
                    printf("You need to be at least level %d to advance!\n", 
                           player->area_level + 1);
                }
            } else if (player->kills >= 10) {
                // Final boss fight at area 5
                printf("\n=== FINAL BOSS CHALLENGE ===\n");
                printf("Do you wish to challenge the final boss? (WARNING: Very difficult!)\n");
                
                if (get_yes_no("Face the final boss?")) {
                    // Create boss enemy with special type
                    Enemy boss;
                    initialize_enemy(&boss, 5, player->level);
                    boss.type = BOSS; // Override to ensure boss type
                    
                    // Fight the boss
                    start_combat(player, &boss);
                    
                    // If player won
                    if (player->hp > 0) {
                        printf("\n=== YOU HAVE COMPLETED THE GAME! ===\n");
                        printf("Congratulations on defeating the final boss!\n");
                        printf("Final stats: Level %d, %d kills, %d gold\n",
                               player->level, player->kills, player->gold);
                    }
                    
                    cleanup_enemy(&boss);
                }
            } else {
                printf("You need to defeat at least 10 monsters before facing the final boss!\n");
                printf("Monsters defeated: %d/10\n", player->kills);
            }
            break;
        }
        
        case 4: // Return to menu
            printf("Returning to main menu.\n");
            break;
            
        default:
            printf("Invalid choice.\n");
    }
}

// Main game loop
void game_loop(Player *player, GameState *state) {
    if (player == NULL || state == NULL) {
        return;
    }
    
    int choice;
    
    switch (*state) {
        case GAME_STATE_MENU:
            printf("\n=== MAIN MENU ===\n");
            printf("1. Explore\n");
            printf("2. Visit Shop\n");
            printf("3. View Character\n");
            printf("4. Save Game\n");
            printf("5. Quit Game\n");
            printf("Choice: ");
            
            if (scanf("%d", &choice) != 1) {
                // Clear invalid input
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                printf("Invalid choice. Please try again.\n");
                return;
            }
            
            // clear newline
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            switch (choice) {
                case 1: // Explore
                    *state = GAME_STATE_EXPLORE;
                    break;
                    
                case 2: // Shop
                    *state = GAME_STATE_SHOP;
                    break;
                    
                case 3: // View Character
                    printf("\n=== CHARACTER INFO ===\n");
                    printf("Name: %s\n", player->name);
                    printf("Class: %s\n", 
                           player->playerClass == PALADIN ? "Paladin" :
                           player->playerClass == ROGUE ? "Rogue" : "Mage");
                    printf("Level: %d\n", player->level);
                    printf("HP: %d/%d\n", player->hp, player->maxHp);
                    printf("Damage: %d\n", player->damage);
                    printf("XP: %d/%d\n", player->xp, player->level * 100);
                    printf("Gold: %d\n", player->gold);
                    printf("Area: %d\n", player->area_level);
                    printf("Kills: %d\n", player->kills);
                    printf("Inventory (%d/%d):\n", player->inventory_size, player->inventory_capacity);
                    
                    if (player->inventory_size == 0) {
                        printf("  Empty\n");
                    } else {
                        for (int i = 0; i < player->inventory_size; i++) {
                            printf("  %d. %s\n", i + 1, player->inventory[i]->name);
                        }
                    }
                    break;
                    
                case 4: // Save Game
                    {
                        if (save_game(player, saveFileName[0] != '\0' ? saveFileName : NULL)) {
                            printf("Game saved successfully!\n");
                        } else {
                            printf("Failed to save game.\n");
                        }
                    }
                    break;
                    
                case 5: // Quit Game
                    if (get_yes_no("Are you sure you want to quit?")) {
                        printf("Thanks for playing!\n");
                        *state = GAME_STATE_GAME_OVER;
                    }
                    break;
                    
                default:
                    printf("Invalid choice. Please try again.\n");
            }
            break;
            
        case GAME_STATE_EXPLORE:
            explore_area(player);
            *state = GAME_STATE_MENU;
            break;
            
        case GAME_STATE_SHOP:
            show_shop(player);
            *state = GAME_STATE_MENU;
            break;
            
        case GAME_STATE_COMBAT:
            // This state is typically handled in start_combat 
            // but we'll add a handler here to avoid the warning
            printf("Returning to menu from combat mode.\n");
            *state = GAME_STATE_MENU;
            break;
            
        case GAME_STATE_GAME_OVER:
        case GAME_STATE_WIN:
            // Game is over, do nothing
            break;
    }
} 