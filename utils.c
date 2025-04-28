#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

// global variable to store log level from environment
static int g_log_level = LOG_ERROR | LOG_COMBAT; // default log stuff

// game difficulty (from env)
static int g_difficulty = 1; // normal difficulty

// enable easter eggs
static bool g_enable_easter_eggs = true;

// this function grabs int from environment... duh
int get_env_int(const char* var_name, int default_val) {
    const char* val = getenv(var_name);
    if (val == NULL) {
        return default_val; // nothin there so use default
    }
    return atoi(val); // string to int or watever
}

// gets string from environment... pretty simple
const char* get_env_string(const char* var_name, const char* default_val) {
    const char* val = getenv(var_name);
    return val != NULL ? val : default_val; // null check or something
}

// gets bool from environment 
// accepts 1/0, true/false, yes/no cuz im nice
bool get_env_bool(const char* var_name, bool default_val) {
    const char* val = getenv(var_name);
    if (val == NULL) {
        return default_val; // if not set return default
    }
    
    // check for "1" or "true" or "yes" case insensitive
    if (val[0] == '1' || 
        (tolower(val[0]) == 't') || 
        (tolower(val[0]) == 'y')) {
        return true;
    }
    // everything else is false i guess
    return false;
}

// init env variables at start
void setup_env_variables() {
    // set log level - example: "GAME_LOG_LEVEL=15" for all logs
    g_log_level = get_env_int("GAME_LOG_LEVEL", g_log_level);
    
    // game difficulty 1=normal, 0=easy, 2=hard
    g_difficulty = get_env_int("GAME_DIFFICULTY", g_difficulty);
    
    // enable easter eggs and funny stuff
    g_enable_easter_eggs = get_env_bool("GAME_EASTER_EGGS", g_enable_easter_eggs);
    
    // log settings if debug is on
    log_event(LOG_DEBUG, "Log level set to 0x%X", g_log_level);
    log_event(LOG_DEBUG, "Game difficulty set to %d", g_difficulty);
    log_event(LOG_DEBUG, "Easter eggs: %s", g_enable_easter_eggs ? "ON" : "OFF");
}

// checks if specific log level bit is enabled
bool is_logging_enabled(int level) {
    return (g_log_level & level) != 0;
}

// log stuff with variable arguments
void log_event(int log_level, const char* format, ...) {
    // dont log if that level isnt enabled
    if (!is_logging_enabled(log_level)) {
        return;
    }
    
    // figure out prefix for different logs
    const char* prefix = "";
    if (log_level & LOG_ERROR) prefix = "[ERROR] ";
    else if (log_level & LOG_COMBAT) prefix = "[COMBAT] ";
    else if (log_level & LOG_DEBUG) prefix = "[DEBUG] ";
    else if (log_level & LOG_FUNNY) prefix = "[LOL] ";
    
    // do the variadic magic
    va_list args;
    va_start(args, format);
    
    printf("%s", prefix); // print prefix
    vprintf(format, args); // print actual message with args
    printf("\n"); // end line
    
    va_end(args);
}

// this is where the real variadic fun happens!!
int cast_spell(Player* caster, Enemy* target, SpellType spell_type, ...) {
    if (caster == NULL) {
        log_event(LOG_ERROR, "Null caster trying to cast spell");
        return 0;
    }
    
    // only mages can cast spells duh
    if (caster->playerClass != MAGE) {
        printf("%s tries to cast a spell but isn't a mage! Nothing happens.\n", caster->name);
        return 0;
    }
    
    va_list args;
    va_start(args, spell_type);
    
    int damage = 0; // how much damage/healing
    
    switch (spell_type) {
        case FIRE_SPELL: {
            // fire takes intensity (1-10) and burns for X turns
            int intensity = va_arg(args, int);
            int burn_turns = va_arg(args, int);
            
            // cap values cuz people will try to cheat
            if (intensity < 1) intensity = 1;
            if (intensity > 10) intensity = 10;
            
            damage = 5 + (intensity * 2);
            
            printf("%s casts FIREBALL (intensity: %d) at %s!\n", 
                   caster->name, intensity, target->name);
            printf("Flames burn for %d turns!\n", burn_turns);
            
            // easter egg for max intensity fire
            if (g_enable_easter_eggs && intensity >= 10) {
                printf("🔥🔥🔥 IT'S SUPER EFFECTIVE! 🔥🔥🔥\n");
            }
            
            log_event(LOG_DEBUG, "Cast fire spell dmg=%d, burn=%d", damage, burn_turns);
            break;
        }
        
        case ICE_SPELL: {
            // ice takes radius (area) and freeze chance
            int radius = va_arg(args, int);
            float freeze_chance = (float)va_arg(args, double); // doubles in varargs!
            
            damage = 3 + (radius * 3);
            
            printf("%s casts FROST NOVA (radius: %d, freeze: %.1f%%) at %s!\n", 
                   caster->name, radius, freeze_chance * 100, target->name);
                   
            // easter egg for big ice spell
            if (g_enable_easter_eggs && radius >= 5) {
                printf("❄️❄️❄️ WINTER IS COMING! ❄️❄️❄️\n");
            }
            
            log_event(LOG_DEBUG, "Cast ice spell dmg=%d, radius=%d, freeze=%.2f", 
                     damage, radius, freeze_chance);
            break;
        }
        
        case LIGHTNING_SPELL: {
            // lightning takes power and number of chain targets
            int power = va_arg(args, int);
            int chain_targets = va_arg(args, int);
            
            damage = power * 4;
            
            printf("%s casts LIGHTNING BOLT (power: %d) at %s!\n", 
                   caster->name, power, target->name);
            printf("Lightning chains to %d additional targets!\n", chain_targets);
            
            // easter egg for high chain lightning
            if (g_enable_easter_eggs && chain_targets >= 3) {
                printf("⚡⚡⚡ UNLIMITED POWER! ⚡⚡⚡\n");
            }
            
            log_event(LOG_DEBUG, "Cast lightning spell dmg=%d, chain=%d", 
                     damage, chain_targets);
            break;
        }
        
        case HEAL_SPELL: {
            // heal takes power and duration (hot)
            int power = va_arg(args, int);
            int duration = va_arg(args, int);
            
            // damage is actually healing for this one
            damage = power * 5;
            
            printf("%s casts HEALING LIGHT (power: %d) on self!\n", caster->name, power);
            printf("Healing continues for %d turns.\n", duration);
            
            // actually heal the player
            caster->hp += damage;
            if (caster->hp > caster->maxHp) {
                caster->hp = caster->maxHp; // dont overheal
            }
            
            // easter egg for big heals
            if (g_enable_easter_eggs && power >= 5) {
                printf("✨✨✨ WELLNESS INTENSIFIES! ✨✨✨\n");
            }
            
            log_event(LOG_DEBUG, "Cast heal spell amount=%d, duration=%d", 
                     damage, duration);
            break;
        }
        
        case RANDOM_SPELL: {
            // completely random spell with random effects lol
            if (g_enable_easter_eggs) {
                const char* random_effects[] = {
                    "turns target into a sheep",
                    "summons dancing skeletons",
                    "creates a pizza out of thin air",
                    "makes everything smell like elderberries",
                    "plays elevator music from nowhere",
                    "causes target to speak in rhymes",
                    "changes gravity direction temporarily"
                };
                
                int effect_index = rand() % (sizeof(random_effects) / sizeof(random_effects[0]));
                damage = rand() % 15 + 1;
                
                printf("%s casts CHAOTIC MAGIC at %s!\n", caster->name, target->name);
                printf("Random effect: %s\n", random_effects[effect_index]);
                
                log_event(LOG_FUNNY, "Random spell cast: %s (dmg=%d)", 
                         random_effects[effect_index], damage);
            } else {
                printf("%s tries to cast random magic, but nothing interesting happens.\n", 
                       caster->name);
                damage = rand() % 5 + 1;
            }
            break;
        }
    }
    
    va_end(args);
    
    // apply difficulty modifier to damage
    if (g_difficulty == 0) { // easy
        damage = damage * 1.5; // 50% more damage
    } else if (g_difficulty == 2) { // hard
        damage = damage * 0.7; // 30% less damage
    }
    
    // return damage dealt
    return damage;
} 