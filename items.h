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

// maybe functions later like create_item?

#endif // ITEMS_H 