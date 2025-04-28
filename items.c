// items.c - Item related functions
#include "items.h"
#include <stdlib.h> // for malloc/free maybe
#include <string.h> // for strcpy maybe
#include <stdarg.h> // for variadic function
#include <stdio.h>  // for printf

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
    
    // Handle item-specific parameters based on type
    switch (itemType) {
        case HEALING: {
            // For healing items, expect one extra int arg (healing amount)
            item->value = va_arg(args, int);
            break;
        }
        
        // Add more item types here as they're implemented
        // case BUFF: ...
        // case CURE: ...
        
        default:
            // Default value if unknown type
            item->value = 1;
            break;
    }
    
    // Cleanup
    va_end(args);
    return item;
}

// Helper function to create a health potion with given strength
Item* create_health_potion(int strength) {
    // Use our variadic function to create the potion
    // The format includes the strength in the name
    return create_item(HEALING, "Health Potion (Strength %d)", strength, strength * 10);
}

// gimme a random potion... idk wut it does lol
Item* create_random_potion() {
    // Random strength between 1-5
    int strength = rand() % 5 + 1;
    
    // Array of funny adjectives (to show using variadic formatting)
    const char* adjectives[] = {
        "Bubbling",
        "Glowing",
        "Fizzy",
        "Questionable",
        "Magical",
        "Sketchy",
        "Ancient",
        "Homemade"
    };
    
    // Pick a random adjective
    int adj_index = rand() % (sizeof(adjectives) / sizeof(adjectives[0]));
    
    // Create the potion with random name and strength
    return create_item(HEALING, "%s Health Potion", adjectives[adj_index], strength * 8);
}

// put item functions here later if needed
// like Item* create_health_potion() { ... } 