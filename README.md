# Simple C RPG

A basic command-line RPG prototype built in C.

[Google Slides Presentation](https://docs.google.com/presentation/d/1p11HXdzm4x1JdRd80vCJWSyHLXaHF7syj5J0WK-eV6g/edit?usp=sharing)

## Core Concept

The player chooses from one of three distinct classes and utilizes their unique abilities in a turn-based system. The game also features consumable items.

## Classes

### Paladin
*   **Holy Strike:** Deals damage and heals the Paladin.
*   **Aura of Light:** Provides a heal-over-time effect for the party (if implemented).
*   **Strike:** Basic damage attack.

### Rogue
*   **Mischief Planning:** Doubles the damage of the next attack.
*   **Poison Dagger:** Applies a damage-over-time effect to the target.
*   **Backstab:** Basic damage attack.

### Mage
*   **Ice Shield:** Reduces incoming damage.
*   **Chain Lightning:** Applies a damage-over-time effect to the target.
*   **Fireball:** Deals damage and may stun the target.

## Consumables

*   **Health Potion:** Restores Hit Points (HP).
*   **Antidote:** Cures poison or other damage-over-time effects.
*   **Strength Potion:** Increases damage output.

## Required C Features Implementation

This section documents code examples for each required C feature in the project.

### 1. Pointers

Pointers are used extensively throughout the code. Example from `enemy.c`:

```c
// From enemy.c (line 90)
enemy->name = malloc(15);
strcpy(enemy->name, "Skeleton");
```

### 2. Strings and String Functions

String operations with functions like `strcmp`, `strcpy`, `strncpy`, and more:

```c
// From main.c (line 37-46)
bool starts_with_insensitive(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) return false;
    
    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);
    
    if (str_len < prefix_len) return false;
    
    for (size_t i = 0; i < prefix_len; i++) {
        if (tolower((unsigned char)str[i]) != tolower((unsigned char)prefix[i])) {
            return false;
        }
    }
    
    return true;
}
```

```c
// From main.c (line 56)
if (strcmp(input, "-fart") == 0 || strcmp(input, "-FART") == 0)
```

### 3. Arrays and Linked Lists

Arrays are used throughout the codebase:

```c
// From player.c (line 183-191) - Inventory as an array of pointers
player->inventory_capacity = INITIAL_INVENTORY_CAPACITY;
player->inventory_size = 0;
// Allocate memory for the array of Item pointers
player->inventory = malloc(sizeof(Item*) * player->inventory_capacity);
if (player->inventory == NULL) {
    // major problem, cant allocate memory
    fprintf(stderr, "Fatal Error: Could not allocate memory for inventory.\n");
    exit(1); // exit the whole game, cant continue
}
```

### 4. Structs and Unions

The game uses several structs to organize data:

```c
// From player.h
typedef struct {
    char* name;                // Dynamically allocated name
    enum PlayerClass playerClass;
    int hp;                    // Current hit points
    int maxHp;                 // Maximum hit points
    int damage;                // Base damage dealt
    int level;                 // Player level
    int xp;                    // Current experience points
    int kills;                 // Kill count for statistics
    int gold;                  // Money for shop things maybe
    int area_level;            // Current dungeon area
    
    // Status effects using bitfields
    unsigned is_poisoned : 1;  // 1 if poisoned
    unsigned is_shielded : 1;  // 1 if has active shield
    unsigned turn_skipped : 1; // 1 if next turn should be skipped
    
    // Inventory
    Item** inventory;          // Array of pointers to Item structs
    int inventory_size;        // Current number of items
    int inventory_capacity;    // Maximum capacity
} Player;
```

### 5. Bitfields

Bitfields are used for player status effects:

```c
// From player.h
// Status effects using bitfields
unsigned is_poisoned : 1;  // 1 if poisoned
unsigned is_shielded : 1;  // 1 if has active shield
unsigned turn_skipped : 1; // 1 if next turn should be skipped
```

### 6. Multiple Source Files

The project is organized into multiple source files:
- main.c
- player.c
- enemy.c
- game.c
- items.c
- utils.c
- save_game.c

### 7. Header Files

Each module has a corresponding header file:
- player.h
- enemy.h
- game.h
- items.h
- utils.h
- save_game.h

### 8. Make or Similar Build Tools

A Makefile is provided to build the project:

```makefile
# From Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = game
SRCS = main.c player.c enemy.c game.c items.c utils.c save_game.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean 
```

### 9. Command Line Options and Arguments

The game supports various command-line arguments:

```c
// From main.c (line 125-150)
if (strcmp(arg, "-name") == 0) {
    if (i + 1 < argc) { // Make sure there's a name after the flag
        strncpy(playerName, argv[i + 1], MAX_NAME_LENGTH - 1);
        playerName[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
        printf("Starting game with player name: %s\n", playerName);
        name_set_from_args = 1;
        i++; // Skip the next argument (the name itself)
    } else {
        fprintf(stderr, "Error: -name flag requires an argument.\n");
        had_invalid_arg = true;
    }
} else if (strcmp(arg, "-save") == 0) {
    if (i + 1 < argc) {
        strncpy(saveFileName, argv[i + 1], MAX_FILENAME_LENGTH - 1);
        saveFileName[MAX_FILENAME_LENGTH - 1] = '\0'; // Ensure null termination
        printf("Save file specified: %s\n", saveFileName);
        i++; // Skip the next argument (the filename)
    } else {
        fprintf(stderr, "Error: -save flag requires a filename argument.\n");
        had_invalid_arg = true;
    }
}
```

### 10. Environment Variables

The game uses environment variables to control behavior:

```c
// From utils.c (line 16-38)
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
```

### 11. malloc() and free()

Memory allocation and deallocation are used throughout the code:

```c
// From player.c (line 26-32)
// copy the name over
player->name = malloc(strlen(name) + 1); // +1 for null terminator
if (player->name == NULL) {
    fprintf(stderr, "Fatal Error: Could not allocate memory for player name.\n");
    // no inventory to free yet
    exit(1); 
}
```

```c
// From player.c (line 178-187)
// free memory allocated for player stuff
void cleanup_player(Player *player) {
    if (player == NULL) return;

    // --- FIX: Free player name if allocated ---
    if (player->name != NULL) {
        printf("Cleaning up player %s...\n", player->name); // Print name before freeing
        free(player->name);
        player->name = NULL;
    } else {
        printf("Cleaning up unnamed player...\n");
    }
}
```

### 12. Variadic Function

The code uses variadic functions for flexible parameter handling:

```c
// From utils.c (line 63-69)
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
```

```c
// From items.c (line 10-60)
// Variadic function to create different items with variable parameters
// itemType: The type of item to create
// nameFormat: Format string for the item name (can include placeholders)
// ...: Variable number of arguments for name formatting and item properties
Item* create_item(enum ItemType itemType, const char* nameFormat, ...) {
    // allocate the item struct
    Item* item = malloc(sizeof(Item));
    if (item == NULL) {
        // malloc fail
        return NULL;
    }
    
    // Set the type
    item->type = itemType;
    
    // Start variadic args processing
    va_list args;
    va_start(args, nameFormat);
    
    // Create a copy of args for name formatting
    va_list args_copy;
    va_copy(args_copy, args);
    
    // First calculate the length of the formatted name
    // using the first set of variable args
    int name_len = vsnprintf(NULL, 0, nameFormat, args_copy);
    va_end(args_copy);
    
    if (name_len < 0) {
        // formatting error
        free(item);
        va_end(args);
        return NULL;
    }
    
    // Allocate space for the name (+1 for null terminator)
    item->name = malloc(name_len + 1);
    if (item->name == NULL) {
        // malloc fail for name
        free(item);
        va_end(args);
        return NULL;
    }
    
    // Now format the name string using the args
    vsnprintf(item->name, name_len + 1, nameFormat, args);
}
```

## Build and Execution

### Building the Project
```bash
make
```

### Running the Game
```bash
./game
```

### Command-line Options
```bash
./game -name YourName -save savefile.csv -god
```

### Environment Variables
You can set environment variables to modify game behavior:
```bash
export GAME_LOG_LEVEL=15   # Enable all logs
export GAME_DIFFICULTY=0   # Easy mode
export CMMO_ENEMY_TYPE=5   # Force dragon enemies
```

### Memory Leak Check
Run with Valgrind to verify no memory leaks:
```bash
valgrind --leak-check=full ./game
```
