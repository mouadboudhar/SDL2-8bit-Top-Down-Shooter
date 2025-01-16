#include "game.h"

Inventory *createInventory(unsigned int inventorySize)
{
    Inventory *inventory = malloc(sizeof(Inventory));
    if (!inventory)
    {
        printf("Failed to allocate memory for inventory\n");
        return NULL; // Error handling
    }
    inventory->inventorySize = inventorySize;
    // Allocate memory for an array of pointers to Item
    inventory->inventory = malloc(sizeof(Item *) * inventorySize);
    if (!inventory->inventory)
    {
        printf("Failed to allocate memory for inventory slots\n");
        free(inventory); // Cleanup if malloc fails
        return NULL;
    }
    // Initialize all inventory slots to NULL
    for (unsigned int i = 0; i < inventorySize; i++)
    {
        inventory->inventory[i] = NULL;
    }
    return inventory;
}

void addItemToInventory(Inventory *inv, Item *item)
{
        for (unsigned int i = 0; i < inv->inventorySize; i++)
        {
            if (inv->inventory[i] == NULL)
            {
                inv->inventory[i] = item;
                return;
            }
        }
}

void printIventory(Inventory *inv)
{
    for (unsigned int i = 0; i < inv->inventorySize; i++)
    {
        if (inv->inventory[i])
        {
            printf("- Slot %d : %s\n", i, inv->inventory[i]->itemName);
        }
    }
}

inventoryItemDisplay *create_node(float x, float y, int i, SDL_Texture *itemSprite, int spriteX, int spriteY)
{
    inventoryItemDisplay *newNode = malloc(sizeof(inventoryItemDisplay));
    if (!newNode)
    {
        printf("Memory allocation failed!\n");
        exit(1); // Exit if memory allocation fails
    }
    newNode->x = x;
    newNode->y = y;
    newNode->inventorySlot = i - 1;
    newNode->itemSprite = itemSprite;
    newNode->spriteX = spriteX;
    newNode->spriteY = spriteY;
    newNode->next = NULL;
    return newNode;
}

void free_inventory_list(inventoryItemDisplay *head)
{
    inventoryItemDisplay *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp); // Free each node
    }
}

inventoryItemDisplay *assign_items_to_slots(Inventory *inv)
{
    if (inv->itemCount <= 0)
        return NULL;

    // Initial positions for the inventory slots
    float x = 10.0f;
    float y = 10.0f;

    // Make sure the first inventory item is valid
    if (inv->inventory[0] == NULL || inv->inventory[0]->itemSprite == NULL)
        return NULL;

    // Create the head node for the linked list
    inventoryItemDisplay *head = create_node(x, y, 1, inv->inventory[0]->itemSprite, inv->inventory[0]->spriteX, inv->inventory[0]->spriteY);
    inventoryItemDisplay *current = head;

    // Create the rest of the nodes
    for (unsigned int i = 2; i <= inv->itemCount; i++)
    {
        // Check for invalid inventory item or null itemSprite
        if (inv->inventory[i - 1]->itemSprite == NULL)
            continue; // Skip to the next item if the current one is invalid

        // Move to the next row after every 8 slots
        x += 13.0f * 9;
        // Create a new node for the current item
        //printf("creating node for item %s, item sprite %p\n, item sprite x: %d, item sprite y: %d\n", inv->inventory[i - 1]->itemName, inv->inventory[i - 1]->itemSprite, inv->inventory[i - 1]->spriteX, inv->inventory[i - 1]->spriteY);
        inventoryItemDisplay *newNode = create_node(x, y, i, inv->inventory[i - 1]->itemSprite, inv->inventory[i - 1]->spriteX, inv->inventory[i - 1]->spriteY);
        if (i % 8 == 0)
        {
            y += 14.0f * 9; // Move down for the next row
            x = -108.0f;    // Reset x position
        }

        // Link the current node to the new node and move the pointer
        current->next = newNode;
        current = newNode;
    }

    return head; // Return the head of the linked list
}

void display_items_in_slots(SDL_Renderer *renderer, inventoryItemDisplay *head, float xOff, float yOff)
{
    inventoryItemDisplay *current = head;
    int c = 0;
    while (current)
    {
        if (current->itemSprite == NULL)
        {
            printf("Null itemSprite at slot %d\n", current->inventorySlot);
            current = current->next;
            continue;
        }
        SDL_Rect srcRect = {current->spriteX, current->spriteY, 14, 14};
        SDL_Rect destRect = {current->x + xOff, current->y + yOff, 70, 70};
        SDL_RenderCopy(renderer, current->itemSprite, &srcRect, &destRect);
        current = current->next;
    }
}

void displayInventory(float x, float y, int width, int height, SDL_Renderer *renderer, SDL_Texture *sprite, SDL_Rect *spriteRect, Inventory *inv, bool displayInv)
{
    inventoryItemDisplay *invDisplay = assign_items_to_slots(inv);
    if (displayInv)
    {
        SDL_Rect destRect;
        destRect.x = x;
        destRect.y = y;
        destRect.w = width;
        destRect.h = height;
        // printf("Inventory oppened\n");
        SDL_RenderCopy(renderer, sprite, spriteRect, &destRect);
        display_items_in_slots(renderer, invDisplay, x + 76, y + 76);
    }
    free_inventory_list(invDisplay);
}