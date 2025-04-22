// enemy.c - Enemy related functions
#include "enemy.h"
#include <stdio.h>

// Initialize a default enemy (e.g., a Goblin)
void initialize_enemy(Enemy *enemy) {
    if (enemy == NULL) {
        fprintf(stderr, "Error: Cannot initialize enemy with NULL pointer.\n");
        return;
    }

    enemy->name = "Goblin"; // Assign pointer to string literal
    enemy->hp = 30;
    enemy->maxHp = 30;
    enemy->damage = 5;

    printf("A wild %s appears! HP: %d/%d, Damage: %d\n", 
           enemy->name, enemy->hp, enemy->maxHp, enemy->damage);
} 