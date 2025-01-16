#include "game.h"

// Generate a unique itemID for each item
int generateUniqueID()
{
    static int counter = 1;
    return counter++;
}

// Create a hash for each itemID
unsigned int hash(char *itemID)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *itemID++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % TABLE_SIZE;
}

// Initialize the table (set everything to NULL)
void initHashTable(Item *table[TABLE_SIZE])
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table[i] = NULL;
    }
}

// Insert an item into the table
void insertItem(Item *table[TABLE_SIZE], Item *item)
{
    unsigned int index = hash(item->itemID);
    item->next = table[index];
    table[index] = item;
}

// Find an item using its item ID
Item *findItem(Item *table[TABLE_SIZE], char *itemID)
{
    unsigned int index = hash(itemID);
    Item *item = table[index];

    while (item != NULL)
    {
        if (strcmp(item->itemID, itemID) == 0)
        {
            return item;
        }
        item = item->next;
    }

    return NULL;
}

Item *findItemByName(Item *table[TABLE_SIZE], char *itemName)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Item *item = table[i]; // Reset item for each bucket

        while (item != NULL)
        {
            if (strcmp(item->itemName, itemName) == 0) // Assuming item->itemID holds the name or ID you're matching
            {
                return item;
            }
            item = item->next; // Move to the next item in the chain (for collision handling)
        }
    }
    return NULL; // Return NULL if no matching item is found
}

// Print out the entire hash table
void printHashTable(Item *table[TABLE_SIZE])
{
    printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        printf("%d - ", i);
        Item *p = table[i];
        if (p == NULL)
        {
            printf("NULL\n");
        }
        else
        {
            while (p != NULL)
            {
                printf("\tName: %s, ItemID: %s -> ", p->itemName, p->itemID);
                p = p->next;
            }
            printf("NULL\n");
        }
    }
}

// Function to create a weapon item
Item *createWeapon(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, weaponType wType, int damage, int bonusDefense, int bonusHealth, int bonusAttack, int bonusSpeed)
{
    Item *item = malloc(sizeof(Item)); // Dynamically allocate memory for the item
    item->type = WEAPON;
    snprintf(item->itemID, 30, "ID%04d", generateUniqueID());
    strncpy(item->itemName, itemName, 30);
    if (!spriteTexture)
        printf("failded to load %s sprite texture\n", item->itemName);
    item->spriteX = spriteX;
    item->spriteY = spriteY;
    item->itemSprite = spriteTexture;
    item->details.weapon.type = wType;
    item->details.weapon.damage = damage;
    item->details.weapon.bonusDefense = bonusDefense;
    item->details.weapon.bonusHealth = bonusHealth;
    item->details.weapon.bonusAttack = bonusAttack;
    item->details.weapon.bonusSpeed = bonusSpeed;
    item->next = NULL;
    return item;
}

// Function to create an armor item
Item *createArmor(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, armorSlot slot, int bonusDefense, int bonusHealth, int bonusAttack, int bonusSpeed)
{
    Item *item = malloc(sizeof(Item)); // Dynamically allocate memory for the item
    item->type = ARMOR;
    snprintf(item->itemID, 30, "ID%04d", generateUniqueID());
    strncpy(item->itemName, itemName, 30);
    if (!spriteTexture)
        printf("failded to load %s sprite texture\n", item->itemName);
    item->spriteX = spriteX;
    item->spriteY = spriteY;
    item->itemSprite = spriteTexture;
    item->details.armor.slot = slot;
    item->details.armor.bonusDefense = bonusDefense;
    item->details.armor.bonusHealth = bonusHealth;
    item->details.armor.bonusAttack = bonusAttack;
    item->details.armor.bonusSpeed = bonusSpeed;
    item->next = NULL;
    return item;
}

// Function to create a consumable item
Item *createConsumable(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, consumableEffect effect, int value)
{
    Item *item = malloc(sizeof(Item)); // Dynamically allocate memory for the item
    item->type = CONSUMABLE;
    snprintf(item->itemID, 30, "ID%04d", generateUniqueID());
    strncpy(item->itemName, itemName, 30);
    if (!spriteTexture)
        printf("failded to load %s sprite texture\n", item->itemName);
    item->spriteX = spriteX;
    item->spriteY = spriteY;
    item->itemSprite = spriteTexture;
    item->details.consumable.effect = effect;
    item->details.consumable.value = value;
    item->next = NULL;
    return item;
}