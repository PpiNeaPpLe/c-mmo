// utils.h - Utility functions and variadic magic
#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include "player.h"
#include "enemy.h"

// log levels (bitmask values so we can combine them)
#define LOG_NONE    0x00
#define LOG_ERROR   0x01  // bad stuff
#define LOG_COMBAT  0x02  // fight stuff
#define LOG_DEBUG   0x04  // for nerds
#define LOG_FUNNY   0x08  // joke logs
#define LOG_ALL     0xFF  // everything

// spell types
typedef enum {
    FIRE_SPELL,
    ICE_SPELL,
    LIGHTNING_SPELL,
    HEAL_SPELL,
    RANDOM_SPELL  // for fun!
} SpellType;

// function prototypes

// get env variable as int with default value
int get_env_int(const char* var_name, int default_val);

// get env variable as string with default value
const char* get_env_string(const char* var_name, const char* default_val);

// get env variable as bool (1/0, true/false, yes/no)
bool get_env_bool(const char* var_name, bool default_val);

// variadic function for logging game events
void log_event(int log_level, const char* format, ...);

// environment variables setup function
void setup_env_variables();

// variadic magic spell function (number of args depends on spell)
int cast_spell(Player* caster, Enemy* target, SpellType spell_type, ...);

// check if logging is enabled for a specific level
bool is_logging_enabled(int level);

#endif // UTILS_H 