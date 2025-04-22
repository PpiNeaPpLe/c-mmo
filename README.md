# Simple C RPG

A basic command-line RPG prototype built in C.

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

## Technical Requirements Checklist

This section tracks the implementation of required C features as per the grading criteria.

*   [ ] Pointers
*   [ ] Strings and at least one string function
*   [ ] At least one of Arrays and Linked Lists
*   [ ] At least one of Structs and Unions
*   [ ] Bitfields (if Structs are used)
*   [ ] Multiple source files
*   [ ] Header files
*   [ ] Make or similar build tools
*   [ ] Command line options and arguments
*   [ ] Environment Variables
*   [ ] `malloc()` and `free()`
*   [ ] Variadic Function

### Build and Execution

*   [ ] Project compiles using `make`.
*   [ ] Program runs without memory leaks (verified with `valgrind`).
