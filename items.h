// items.h - Item definitions
#ifndef ITEMS_H
#define ITEMS_H

// what kind of item is it?
enum ItemType {
    HEALING, // like a potion
    // BUFF,    // like str potion (later maybe)
    // CURE     // like antidote (later maybe)
};

// the item itself
typedef struct {
    char *name;         // item name, maybe needs malloc?
    enum ItemType type; // what it does
    int value;          // how much heal/buff etc
} Item;

// Variadic function to create items with custom properties
Item* create_item(enum ItemType itemType, const char* nameFormat, ...);

// Helper functions using the variadic create_item
Item* create_health_potion(int strength);
Item* create_random_potion();

#endif // ITEMS_H 