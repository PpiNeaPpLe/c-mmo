#include <stdio.h>
#include <string.h>

int main() {
    char playerName[50];

    printf("====================================\n");
    printf("      Welcome to Simple RPG!\n");
    printf("====================================\n");
    printf("Choose your class:\n");
    printf("- Rogue\n");
    printf("- Mage \n");
    printf("- Paladin \n\n");

    printf("Enter your name, adventurer: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = '\0'; // Remove newline

    printf("\nGreetings, %s! Long ago, the Crown of Light kept the kingdom of Eldergarde in peace—until the paladin Malakar, consumed by greed, stole it and twisted its power into something dark. Now, the land withers. Monsters stalk the villages. And whispers speak of a shadow creeping from the Ashen Keep, where Malakar waits. You, a fledgling adventurer, arrive in Eldergarde with nothing but your wits and your steel. The villagers’ eyes turn to you, desperate for hope. Will you answer the call?\n", playerName);
    printf("(Press Enter to continue...)");
    getchar(); // Wait for input before closing (optional)

    return 0;
}