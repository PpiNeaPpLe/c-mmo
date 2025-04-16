#include <stdio.h>
#include <string.h>

int main() 
{
    char playerName[50];

    // Display game title and welcome message
    printf("====================================\n");
    printf("      Welcome to Simple RPG!\n");
    printf("====================================\n\n");
    
    // Display class options
    printf("Choose your class:\n");
    printf("- Rogue\n");
    printf("- Mage\n");
    printf("- Paladin\n\n");
    
    // Get player name
    printf("Enter your name, adventurer: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = '\0';  // Remove newline character
    
    // Display story introduction
    printf("\nGreetings, %s!\n\n", playerName);
    printf("Long ago, the Crown of Light kept the kingdom of Eldergarde in peace\n"
           "—until the paladin Malakar, consumed by greed, stole it and twisted its\n"
           "power into something dark. Now, the land withers. Monsters stalk the\n"
           "villages. And whispers speak of a shadow creeping from the Ashen Keep,\n"
           "where Malakar waits.\n\n"
           "You, a fledgling adventurer, arrive in Eldergarde with nothing but your\n"
           "wits and your steel. The villagers' eyes turn to you, desperate for hope.\n"
           "Will you answer the call?\n\n");
    
    // Wait for user to continue
    printf("(Press Enter to continue...)");
    getchar();
    
    return 0;
}
