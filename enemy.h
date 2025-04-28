// enemy.h - Header for enemy structure and functions
#ifndef ENEMY_H
#define ENEMY_H

// enemy types
enum EnemyType {
    GOBLIN,        // weak but scrappy
    SKELETON,      // harder but not too bad
    ZOMBIE,        // slow but strong
    TROLL,         // big & tough
    ORC,           // strong warriors
    DRAGON,        // super hard boss!!
    BOSS           // final boss maybe?
};

// Basic Enemy structure
typedef struct {
    char *name; // Using a pointer for the name
    int hp;
    int maxHp;
    int damage;
    enum EnemyType type;
    int level;       // enemy level
    int xp_value;    // how much XP they give
    int gold_value;  // how much gold they drop
} Enemy;

// Function prototypes for enemy actions will go here
// For example:
// void initialize_enemy(Enemy *enemy, const char *name, int hp, int damage);

// Create an enemy based on area level and player level
void initialize_enemy(Enemy *enemy, int area_level, int player_level);

// clean up enemy resources
void cleanup_enemy(Enemy *enemy);

// get a random enemy type based on area level
enum EnemyType get_random_enemy_type(int area_level);

#endif // ENEMY_H 