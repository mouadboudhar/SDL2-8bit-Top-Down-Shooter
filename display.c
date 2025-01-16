#include "game.h"

tmx_object *detectPlayerObjectCollision(tmx_map *map, RigidBody *player, const char *itemName)
{
    ObjectNode *objects = findObject(map, "items", itemName);
    ObjectNode *current = objects;
    while (current)
    {
        tmx_object *object = current->object;
        if (player->x + RECT_WIDTH <= object->x * MAP_SCALE ||  // Player is to the left of the item
            player->x >= object->x * MAP_SCALE + ITEM_WIDTH ||  // Player is to the right of the item
            player->y + RECT_HEIGHT <= object->y * MAP_SCALE || // Player is above the item
            player->y >= object->y * MAP_SCALE + ITEM_HEIGHT)   // Player is below the item
        {
            current = current->next;
            continue; // No collision with this object, check next
        }

        // Collision detected with this object
        // printf("Collision detected with object: %s\n", itemName);
        return object;
    }

    return NULL; // No collision detected with any object
}

ObjectNode *findObject(tmx_map *map, const char *layer_name, const char *object_name)
{
    tmx_layer *layer = map->ly_head; // Start with the first layer
    ObjectNode *head = NULL, *tail = NULL;

    // Loop through each layer of the map to find the "items" object layer
    while (layer)
    {
        if (layer->type == L_OBJGR && strcmp(layer->name, layer_name) == 0)
        {
            tmx_object *obj = layer->content.objgr->head; // Get the first object in the layer

            // Loop through each object in the object group
            while (obj)
            {
                if (obj->name && strcmp(obj->name, object_name) == 0)
                {
                    // Create a new node and add it to the list
                    ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
                    node->object = obj;
                    node->next = NULL;

                    if (tail)
                    {
                        tail->next = node;
                        tail = node;
                    }
                    else
                    {
                        head = tail = node;
                    }
                }
                obj = obj->next; // Move to the next object
            }
        }
        layer = layer->next; // Move to the next layer
    }

    return head; // Return the list of objects
}

ObjectNode *findAllObject(tmx_map *map, const char *layer_name)
{
    tmx_layer *layer = map->ly_head; // Start with the first layer
    ObjectNode *head = NULL, *tail = NULL;

    // Loop through each layer of the map to find the "items" object layer
    while (layer)
    {
        if (layer->type == L_OBJGR && strcmp(layer->name, layer_name) == 0)
        {
            tmx_object *obj = layer->content.objgr->head; // Get the first object in the layer

            // Loop through each object in the object group
            while (obj)
            {
                // Create a new node and add it to the list
                ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
                node->object = obj;
                node->next = NULL;

                if (tail)
                {
                    tail->next = node;
                    tail = node;
                }
                else
                {
                    head = tail = node;
                }
                obj = obj->next; // Move to the next object
            }
        }
        layer = layer->next; // Move to the next layer
    }
    // ObjectNode* current = head;
    // int c = 0;
    // while(current)
    // {
    //     c++;
    //     printf("%d - %s",c, current->object->name);
    //     current = current->next;
    // }

    return head; // Return the list of objects
}

// void displayItem(SDL_Renderer *renderer, const char *fontPath, int fontSize, tmx_map *map, SDL_Texture *sprite, RigidBody *player, SDL_Rect *spriteRect, Inventory *inv, Item *item, bool interact, Camera *camera)
// {
//     // Create ItemDisplay instance
//     ObjectNode *objects = findObject(map, "items", item->itemName);
//     ObjectNode *current = objects;
//     while (current)
//     {
//         if (current == NULL)
//         {
//             printf("Object not found: %s\n", item->itemName);
//             return; // Safely exit if current is not found
//         }

//         // Create destination rectangle
//         SDL_Rect destRect;
//         destRect.x = ((current->object->x * MAP_SCALE) - camera->x);
//         destRect.y = ((current->object->y * MAP_SCALE) - camera->y);
//         destRect.w = ITEM_WIDTH;
//         destRect.h = ITEM_HEIGHT;

//         tmx_property *prop = tmx_get_property(current->object->properties, "active");
//         if (prop != NULL && prop->type == PT_BOOL)
//         {
//             if (detectPlayerObjectCollision(map, player, item->itemName) == current->object && prop->value.boolean && interact)
//             {
//                 if (inv->itemCount < inv->inventorySize)
//                 {
//                     printf("%s added to inventory\n", item->itemName);
//                     addItemToInventory(inv, item);
//                     printIventory(inv);
//                     inv->itemCount += 1;
//                     printf("%u items in inventory\n", inv->itemCount);
//                     // healPlayer(player, item->details.consumable.value);
//                     prop->value.boolean = false;
//                 }
//                 else
//                     printf("Inventory is full\n");
//             }

//             if (prop->value.boolean)
//             {
//                 SDL_RenderCopy(renderer, sprite, spriteRect, &destRect);
//                 displayText(renderer, fontPath, fontSize, item->itemName, destRect.x - 20, destRect.y + 60, (SDL_Color){255, 255, 255, 255});
//             }
//         }
//         current = current->next;
//     }
//     while (objects)
//     {
//         ObjectNode *temp = objects;
//         objects = objects->next;
//         free(temp);
//     }
// }

// GridCell **createGrid(int mapWidth, int mapHeight)
// {
//     // Calculate the number of cells in the grid
//     int gridWidth = (mapWidth / CELL_SIZE) + 1;
//     int gridHeight = (mapHeight / CELL_SIZE) + 1;

//     // Allocate memory for the grid
//     GridCell **grid = (GridCell **)malloc(gridWidth * sizeof(GridCell *));
//     for (int i = 0; i < gridWidth; ++i)
//     {
//         grid[i] = (GridCell *)malloc(gridHeight * sizeof(GridCell));
//         for (int j = 0; j < gridHeight; ++j)
//         {
//             grid[i][j].items = NULL; // Initialize empty cells
//         }
//     }
//     return grid;
// }

// void addItemToGrid(GridCell **grid, ObjectNode *item, int mapWidth, int mapHeight)
// {
//     int gridX = item->object->x / CELL_SIZE;
//     int gridY = item->object->y / CELL_SIZE;

//     // Add the item to the appropriate grid cell
//     item->next = grid[gridX][gridY].items;
//     grid[gridX][gridY].items = item;
// }
// Function to get the grid coordinates for a position

int getGridIndex(int position, int cellSize)
{
    return position / cellSize;
}

void displayItems(SDL_Renderer *renderer, const char *fontPath, int fontSize, tmx_map *map, Item *table[TABLE_SIZE], RigidBody *player, Inventory *inv, bool interact, Camera *camera)
{
    // Create ItemDisplay instance
    ObjectNode *objects = findAllObject(map, "items");
    ObjectNode *current = objects;

    // Calculate the grid boundaries based on the camera's position
    int startX = getGridIndex(camera->x, GRID_CELL_SIZE);
    int startY = getGridIndex(camera->y, GRID_CELL_SIZE);
    int endX = getGridIndex(camera->x + camera->width, GRID_CELL_SIZE);
    int endY = getGridIndex(camera->y + camera->height, GRID_CELL_SIZE);

    while (current)
    {
        if (current == NULL)
        {
            printf("No objects found\n");
            return; // Safely exit if current is not found
        }

        // Get object's grid position
        int objGridX = getGridIndex(current->object->x * MAP_SCALE, GRID_CELL_SIZE);
        int objGridY = getGridIndex(current->object->y * MAP_SCALE, GRID_CELL_SIZE);

        // Only process the object if it's within the camera's grid area
        if (objGridX >= startX && objGridX <= endX && objGridY >= startY && objGridY <= endY)
        {
            
            // Create destination rectangle
            SDL_Rect destRect;
            destRect.x = ((current->object->x * MAP_SCALE) - camera->x);
            destRect.y = ((current->object->y * MAP_SCALE) - camera->y);
            destRect.w = ITEM_WIDTH;
            destRect.h = ITEM_HEIGHT;

            tmx_property *prop = tmx_get_property(current->object->properties, "active");
            if (prop != NULL && prop->type == PT_BOOL)
            {
                Item *foundItem = findItemByName(table, current->object->name);
                SDL_Rect spriteRect = {foundItem->spriteX, foundItem->spriteY, 14, 14};
                if (foundItem == NULL)
                {
                    printf("Error: Item '%s' not found.\n", current->object->name);
                }
                else
                {
                    // Collision detection and item interaction
                    if (detectPlayerObjectCollision(map, player, current->object->name) == current->object && prop->value.boolean && interact)
                    {
                        if (inv->itemCount < inv->inventorySize)
                        {
                            printf("%s added to inventory\n", current->object->name);
                            addItemToInventory(inv, findItemByName(table, current->object->name));
                            printIventory(inv);
                            inv->itemCount += 1;
                            printf("%u items in inventory\n", inv->itemCount);
                            prop->value.boolean = false;
                        }
                        else
                        {
                            printf("Inventory is full\n");
                        }
                    }

                    // Render the item if it's active and within camera view
                    if (foundItem->itemSprite != NULL && prop->value.boolean && destRect.x < camera->x + camera->width && destRect.y < camera->y + camera->height)
                    {
                        SDL_Texture *itemSprite = foundItem->itemSprite;
                        SDL_RenderCopy(renderer, itemSprite, &spriteRect, &destRect);
                        displayText(renderer, fontPath, fontSize, current->object->name, destRect.x - 20, destRect.y + 60, (SDL_Color){255, 255, 255, 255});
                    }
                }
            }
        }
        current = current->next;
    }

    while (objects)
    {
        ObjectNode *temp = objects;
        objects = objects->next;
        free(temp);
    }
}