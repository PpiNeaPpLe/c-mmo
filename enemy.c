// enemy.c - Enemy related functions
#include "enemy.h"
#include <stdio.h>
#include <stdlib.h> // Need this for getenv and atoi

// Initialize a default enemy (e.g., a Goblin)
void initialize_enemy(Enemy *enemy) {
    if (enemy == NULL) {
        fprintf(stderr, "Error: Cannot initialize enemy with NULL pointer.\n");
        return;
    }

    enemy->name = "Goblin"; // Assign pointer to string literal
    enemy->damage = 5;
    
    // Check for environment variable for HP
    int start_hp = 30; // Default HP
    char *hp_env = getenv("RPG_ENEMY_HP"); // Check env var

    if (hp_env != NULL) {
        int env_hp = atoi(hp_env); // Convert string to int
        if (env_hp > 0) { // Basic validation
            start_hp = env_hp;
            printf("[Debug] Enemy HP set from environment: %d\n", start_hp); // Optional debug msg
        } else {
             printf("[Debug] Invalid RPG_ENEMY_HP value ('%s'), using default %d.\n", hp_env, start_hp);
        }
    }

    enemy->hp = start_hp;
    enemy->maxHp = start_hp;

    printf("A wild %s appears! HP: %d/%d, Damage: %d\n", 
           enemy->name, enemy->hp, enemy->maxHp, enemy->damage);
} 