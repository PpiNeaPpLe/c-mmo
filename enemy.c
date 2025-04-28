// enemy.c - Enemy related functions
#include "enemy.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h> // Need this for getenv and atoi
#include <string.h> // for strcpy etc
#include <time.h>   // for rand

// Get a random enemy type appropriate for the area level
enum EnemyType get_random_enemy_type(int area_level) {
    int max_type = 0;
    
    // each area has certain enemy types available
    switch (area_level) {
        case 1:  // starting area
            max_type = SKELETON; // only goblins and skeletons
            break;
        case 2:  // harder area
            max_type = ZOMBIE;   // up to zombies
            break;
        case 3:  // harder
            max_type = TROLL;    // up to trolls
            break;
        case 4:  // even harder
            max_type = ORC;      // up to orcs
            break;
        case 5:  // hardest area
            // special case to handle dragon sparsity
            if (rand() % 10 == 0) { // 10% chance
                return DRAGON;   // rare dragon spawn
            }
            max_type = ORC;      // mostly orcs
            break;
        default: // invalid area, use goblin
            return GOBLIN;
    }
    
    // return a random enemy type from GOBLIN to max_type
    return (enum EnemyType)(rand() % (max_type + 1));
}

// Initialize an enemy based on area level and player level
void initialize_enemy(Enemy *enemy, int area_level, int player_level) {
    if (enemy == NULL) {
        fprintf(stderr, "Error: Cannot initialize enemy with NULL pointer.\n");
        return;
    }
    
    // make sure area and player levels are valid
    if (area_level < 1) area_level = 1;
    if (area_level > 5) area_level = 5;
    if (player_level < 1) player_level = 1;
    
    // get a random enemy type based on area
    enemy->type = get_random_enemy_type(area_level);
    
    // Check for environment variable override for enemy type
    char *type_env = getenv("CMMO_ENEMY_TYPE");
    if (type_env != NULL) {
        int env_type = atoi(type_env);
        if (env_type >= GOBLIN && env_type <= BOSS) {
            enemy->type = (enum EnemyType)env_type;
            printf("[Debug] Enemy type set from environment: %d\n", env_type);
        }
    }
    
    // special boss case - if env var says BOSS type
    bool is_boss = (enemy->type == BOSS);
    
    // set enemy properties based on type
    switch (enemy->type) {
        case GOBLIN:
            enemy->name = malloc(10); // alloc memory for name
            strcpy(enemy->name, "Goblin");
            enemy->level = player_level;
            enemy->hp = 20 + (5 * enemy->level); 
            enemy->damage = 3 + (enemy->level);
            enemy->xp_value = 20 + (5 * enemy->level);
            enemy->gold_value = 5 + (enemy->level * 2);
            break;
            
        case SKELETON:
            enemy->name = malloc(15);
            strcpy(enemy->name, "Skeleton");
            enemy->level = player_level + 1;
            enemy->hp = 25 + (6 * enemy->level);
            enemy->damage = 5 + (enemy->level);
            enemy->xp_value = 30 + (7 * enemy->level);
            enemy->gold_value = 8 + (enemy->level * 2);
            break;
            
        case ZOMBIE:
            enemy->name = malloc(15);
            strcpy(enemy->name, "Zombie");
            enemy->level = player_level + 1;
            enemy->hp = 40 + (8 * enemy->level);
            enemy->damage = 4 + (enemy->level);
            enemy->xp_value = 40 + (8 * enemy->level);
            enemy->gold_value = 10 + (enemy->level * 2);
            break;
            
        case TROLL:
            enemy->name = malloc(15);
            strcpy(enemy->name, "Troll");
            enemy->level = player_level + 2;
            enemy->hp = 60 + (10 * enemy->level);
            enemy->damage = 7 + (enemy->level * 2);
            enemy->xp_value = 60 + (10 * enemy->level);
            enemy->gold_value = 15 + (enemy->level * 3);
            break;
            
        case ORC:
            enemy->name = malloc(15);
            strcpy(enemy->name, "Orc Warrior");
            enemy->level = player_level + 2;
            enemy->hp = 50 + (8 * enemy->level);
            enemy->damage = 8 + (enemy->level * 2);
            enemy->xp_value = 70 + (12 * enemy->level);
            enemy->gold_value = 20 + (enemy->level * 3);
            break;
            
        case DRAGON:
            enemy->name = malloc(20);
            strcpy(enemy->name, "Fire Dragon");
            enemy->level = player_level + 4;
            enemy->hp = 150 + (15 * enemy->level);
            enemy->damage = 15 + (enemy->level * 3);
            enemy->xp_value = 200 + (20 * enemy->level);
            enemy->gold_value = 100 + (enemy->level * 10);
            break;
            
        case BOSS:
            enemy->name = malloc(25);
            strcpy(enemy->name, "Dungeon Master");
            enemy->level = player_level + 5;
            enemy->hp = 300 + (20 * enemy->level);
            enemy->damage = 20 + (enemy->level * 3);
            enemy->xp_value = 500 + (50 * enemy->level);
            enemy->gold_value = 200 + (enemy->level * 20);
            break;
            
        default: // failsafe goblin lol
            enemy->name = malloc(10);
            strcpy(enemy->name, "Goblin");
            enemy->level = player_level;
            enemy->hp = 20 + (5 * enemy->level);
            enemy->damage = 3 + enemy->level;
            enemy->xp_value = 20 + (5 * enemy->level);
            enemy->gold_value = 5 + (enemy->level * 2);
    }
    
    // Check for environment variable for HP override
    char *hp_env = getenv("CMMO_ENEMY_HP");
    if (hp_env != NULL) {
        int env_hp = atoi(hp_env); // Convert string to int
        if (env_hp > 0) { // Basic validation
            enemy->hp = env_hp;
            enemy->maxHp = env_hp;
            printf("[Debug] Enemy HP set from environment: %d\n", env_hp);
        }
    } else {
        enemy->maxHp = enemy->hp; // maxHp same as starting hp
    }
    
    // scale enemy damage based on difficulty
    int difficulty = get_env_int("GAME_DIFFICULTY", 1);
    if (difficulty == 0) { // easy
        enemy->damage = enemy->damage * 0.7; // 30% less damage
    } else if (difficulty == 2) { // hard
        enemy->damage = enemy->damage * 1.3; // 30% more damage
    }
    
    printf("A level %d %s appears! HP: %d/%d, Damage: %d\n", 
           enemy->level, enemy->name, enemy->hp, enemy->maxHp, enemy->damage);
    
    // special message for boss fight
    if (is_boss) {
        printf("\n🔥🔥🔥 BOSS FIGHT!!! 🔥🔥🔥\n");
        printf("The %s laughs menacingly...\n\n", enemy->name);
    }
}

// free memory used by enemy
void cleanup_enemy(Enemy *enemy) {
    if (enemy == NULL) {
        return;
    }
    
    // free name if it was dynamically allocated
    if (enemy->name != NULL) {
        free(enemy->name);
        enemy->name = NULL;
    }
    
    // reset values for safety
    enemy->hp = 0;
    enemy->maxHp = 0;
    enemy->damage = 0;
    enemy->level = 0;
} 