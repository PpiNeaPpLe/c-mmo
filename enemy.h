// enemy.h - Header for enemy structure and functions
#ifndef ENEMY_H
#define ENEMY_H

// Basic Enemy structure
typedef struct {
    char *name; // Using a pointer for the name
    int hp;
    int maxHp;
    int damage;
} Enemy;

// Function prototypes for enemy actions will go here
// For example:
// void initialize_enemy(Enemy *enemy, const char *name, int hp, int damage);
void initialize_enemy(Enemy *enemy);

#endif // ENEMY_H 