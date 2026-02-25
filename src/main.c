#include "game.h"

// The renderer
SDL_Renderer *renderer;
// Godmode for testing
bool godmode = false;
// The hashtable to store the items
Item *table[TABLE_SIZE];

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    if (argc > 1 && argv[0])
    {
        godmode = true;
        printf("\ngodmode\n");
    }
    SDL_Window *window = SDL_CreateWindow("Dungeons And Sorcery", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    if (window == NULL)
    {
        SDL_Log("SDL_CreateWindow ERROR: %s \n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("SDL_CreateRenderer ERROR: %s \n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 2;
    }

    SDL_Texture *spriteSheetTexture = loadTexture("assets/sprites/spritesheet3.png", renderer);
    if (spriteSheetTexture == NULL)
    {
        SDL_Log("Failed to load sprite sheet texture: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    SDL_Texture *uiTextures = loadTexture("assets/sprites/hpbar.png", renderer);
    if (uiTextures == NULL)
    {
        SDL_Log("Failed to load ui texture: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    SDL_Texture *avatarTexture = loadTexture("assets/sprites/MageAvatar2framed.png", renderer);
    if (avatarTexture == NULL)
    {
        SDL_Log("Failed to load avatar texture: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 5;
    }

    SDL_Texture *hpPotTexture = loadTexture("assets/sprites/item_icons.png", renderer);
    if (avatarTexture == NULL)
    {
        SDL_Log("Failed to load Health Pot texture: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 5;
    }
    SDL_Texture *projectileSpriteSheet = loadTexture("assets/sprites/projectileSpriteSheet.png", renderer);
    if (projectileSpriteSheet == NULL)
    {
        SDL_Log("Failed to load projectile sprite sheet texture: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init ERROR: %s \n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return 4;
    }

    const char *fontPath = "assets/fonts/AncientModernTales-a7Po.ttf";

    int screenWidth, screenHeight;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);

    printf("\nSW: %d SH: %d\n", screenWidth, screenHeight);

    tmx_map *map = mapLoader("assets/maps/dungeon/dungeonRoom1.tmx");

    RigidBody *player = initBody(screenWidth / 2, screenHeight / 2, 0, 0, 9.0f, 13.0f, 0.7f, 30, 1000, 1000, true);
    unsigned int inventorySize = 32;
    Inventory *inventory = createInventory(inventorySize);

    RigidBody *
        enemies[MAX_ENEMIES] = {NULL}; // Initialize enemy pointers to NULL
    Projectile projectiles[MAX_PROJECTILES] = {0};
    Camera camera = {0, 0, screenWidth, screenHeight};

    // Create items
    initHashTable(table);
    Item *HealthPotion = createConsumable("Health Potion", loadTexture("assets/sprites/item_icons.png", renderer), 81, 1, HEALING, player->maxhealth * 20 / 100);
    insertItem(table, HealthPotion);
    Item *ManaPotion = createConsumable("Mana Potion", loadTexture("assets/sprites/item_icons.png", renderer), 97, 1, HEALING, player->health * 20 / 100);
    insertItem(table, ManaPotion);
    Item *StaffPlaceholder = createWeapon("Staff Placeholder", loadTexture("assets/sprites/item_icons.png", renderer), 1, 65, STAFF, 9999, 9999, 9999, 9999, 0);
    insertItem(table, StaffPlaceholder);
    Item *VoidBlade = createWeapon("Void Blade", loadTexture("assets/sprites/void_blade.png", renderer), 0, 0, SWORD, 9999, 9999, 9999, 9999, 0);
    insertItem(table, VoidBlade);
    printHashTable(table);
    // Iventory Display
    inventoryDisplay invDisplay;
    invDisplay.inventoryTexture = loadTexture("assets/sprites/inventory_32.png", renderer);
    int rectW, rectH;
    SDL_QueryTexture(invDisplay.inventoryTexture, NULL, NULL, &rectW, &rectH);
    invDisplay.width = rectW * 9;
    invDisplay.height = rectH * 9;
    invDisplay.x = screenWidth / 2 - invDisplay.width / 2;
    invDisplay.y = screenHeight / 2 - invDisplay.height / 2;
    SDL_Rect invRect = {0, 0, rectW, rectH};
    invDisplay.inventoryRect = invRect;

    int up = 0,
        down = 0, left = 0, right = 0, running = 1;
    int enemyCount = 0;
    SDL_Event event;

    Uint32 lastShotTime = 0;           // Initialize last shot time
    float shootingInterval = 1000 / 5; // Shooting interval for 3 shots per second (in milliseconds)

    int charSpriteX = 0;
    int charSpriteY = 0;
    int enemySpriteX = 88;
    int enemySpriteY = 0;
    int lastCharSpriteX = 0;
    int lastCharSpriteY = 0;
    int lastDirectionFaced = 0;

    SDL_RendererFlip flipPlayer = SDL_FLIP_NONE;
    SDL_RendererFlip flipEnemy = SDL_FLIP_NONE;

    int timer = 0;
    int currentFrame = 0;
    int enemyCurrentFrame = 0;
    int animationTimer = 0;
    int enemyAnimationTimer = 0;

    int scoreCount = 0;

    float hpPotX = 300.0f;
    float hpPotY = 300.0f;

    bool displayInv = false;
    bool pause = false;
    bool interact = false;

    bool autoshoot = false;

    inventory->itemCount = 0;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int projectileMouseX, projectileMouseY;

    int mapScale = 5;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
            else if (event.type == SDL_KEYDOWN)
            {
                // Handle key presses for movement (these should still work when the inventory is open)
                if (event.key.keysym.sym == SDLK_w)
                    up = 1;
                if (event.key.keysym.sym == SDLK_s)
                    down = 1;
                if (event.key.keysym.sym == SDLK_a)
                    left = 1;
                if (event.key.keysym.sym == SDLK_d)
                    right = 1;

                if (event.key.keysym.sym == SDLK_b)
                    displayInv = !displayInv;

                if (event.key.keysym.sym == SDLK_p)
                    pause = !pause;

                if (event.key.keysym.sym == SDLK_e)
                    interact = true;

                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;

                if (event.key.keysym.sym == SDLK_i)
                    autoshoot = !autoshoot;
            }
            else if (event.type == SDL_KEYUP)
            {
                // Handle key releases for movement
                if (event.key.keysym.sym == SDLK_w)
                    up = 0;
                if (event.key.keysym.sym == SDLK_s)
                    down = 0;
                if (event.key.keysym.sym == SDLK_a)
                    left = 0;
                if (event.key.keysym.sym == SDLK_d)
                    right = 0;
                if (event.key.keysym.sym == SDLK_e)
                    interact = false;
            }
        }
        if (!pause)
        {
            timer++;
            // if (timer % 16 == 0) printf("\ncurrent speed = x= %f y= %f", player->velX, player->velY);
            //  printf("\ntimer = %ds", seconds++);
            Uint32 animationStartTime = SDL_GetTicks();
            Uint32 animationInterval = 100; // milliseconds per frame

            // Inside your game loop
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - animationStartTime >= animationInterval)
            {
                currentFrame = (currentFrame + 1) % NUM_FRAMES;
                animationStartTime = currentTime; // Reset the timer
            }
            // Update animation frame if the player is moving
            if (up || down || left || right)
            {
                animationTimer++;
                if (animationTimer % FRAME_RATE == 0)
                {
                    currentFrame = (currentFrame + 1) % NUM_FRAMES;
                }

                // Update sprite direction
                if (up)
                {
                    charSpriteX = 72;
                    charSpriteY = 0;
                    lastDirectionFaced = 1;
                }
                else if (down)
                {
                    charSpriteX = 48;
                    charSpriteY = 0;
                    lastDirectionFaced = 2;
                }
                else if (left)
                {
                    charSpriteX = 8;
                    charSpriteY = 0;
                    flipPlayer = SDL_FLIP_HORIZONTAL;
                    lastDirectionFaced = 3;
                }
                else if (right)
                {
                    charSpriteX = 8;
                    charSpriteY = 0;
                    flipPlayer = SDL_FLIP_NONE;
                    lastDirectionFaced = 4;
                }

                // Update last direction coordinates
                lastCharSpriteX = charSpriteX;
                lastCharSpriteY = charSpriteY;
            }
            else
            {
                if (lastDirectionFaced == 1)
                    charSpriteX = 64;
                if (lastDirectionFaced == 2)
                    charSpriteX = 32;
                if (lastDirectionFaced == 3 || lastDirectionFaced == 4)
                    charSpriteX = 0;
                currentFrame = 0;
                animationTimer = 0; // Reset animation timer
            }

            // Clear the screen
            SDL_SetRenderDrawColor(renderer, 69, 69, 69, 255);
            SDL_RenderClear(renderer);

            // Rendering the tilemap
            render_map(map, &camera, mapScale);

            // Check if the left mouse button is currently pressed
            const Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
            if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) || autoshoot)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                shootProjectile(projectiles, player, mouseX, mouseY, &camera, &lastShotTime, shootingInterval);
            }

            // Initialize and ender projectiles
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    updateProjectiles(&projectiles[i]);
                    // Check if the projectile goes out of bounds
                    if (projectiles[i].x < 0 || projectiles[i].x >= MAP_WIDTH || projectiles[i].y < 0 || projectiles[i].y >= MAP_HEIGHT)
                    {
                        projectiles[i].active = 0;
                    }
                    // Check if the projectile collided with any map collisions.
                    else if (projectile_collision_detection(&projectiles[i], map, mapScale) == 1)
                    {
                        projectiles[i].active = 0;
                    }
                    // Check if the projectile collided with any enemies
                    else
                    {
                        checkCollisionProjectileEnemy(&projectiles[i], enemies, enemyCount, &scoreCount);
                    }
                }
            }

            SDL_GetMouseState(&projectileMouseX, &projectileMouseY);
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    displayProjectile(renderer, &projectiles[i], projectileSpriteSheet, 16, 16, &camera, SDL_FLIP_NONE, 5, 4.0); // 5 frames, 100ms delay per frame
                }
            }

            // Init all the enemies.
            spawnEnemy(enemies, player, &enemyCount, mapScale, map, &camera, SDL_GetTicks());

            // Draw the Items
            //(renderer, fontPath, 20, map, HealthPotion->itemSprite, player, &itemSpriteRect, inventory, HealthPotion, interact, &camera);
            displayItems(renderer, fontPath, 20, map, table, player, inventory, interact, &camera);

            // Render and move the player.
            SDL_Rect playerSpriteRect = {charSpriteX + currentFrame * SPRITE_SIZE,
                                         charSpriteY,
                                         SPRITE_SIZE,
                                         SPRITE_SIZE};
            updateCamera(&camera, player); // Update the Camera.
            movePlayer(player, map, mapScale, up, down, left, right);
            displayBody(renderer, player, spriteSheetTexture, &playerSpriteRect, RECT_WIDTH, RECT_HEIGHT, &camera, flipPlayer);
            // Animation player for the player.
            if (currentTime - animationStartTime >= animationInterval)
            {
                enemyCurrentFrame = (enemyCurrentFrame + 1) % ENEMY_NUM_FRAMES;
                animationStartTime = currentTime; // Reset the timer
            }
            // Draw the player's health bar under the player character.
            drawHealthBar(player, renderer, &camera);

            // Render and move the enemies.
            SDL_Rect enemySpriteRect = {enemySpriteX + enemyCurrentFrame * SPRITE_SIZE, enemySpriteY, SPRITE_SIZE, SPRITE_SIZE};
            for (int i = 0; i < enemyCount; ++i)
            {
                if (enemies[i] != NULL)
                {
                    displayBody(renderer, enemies[i], spriteSheetTexture, &enemySpriteRect, RECT_WIDTH, RECT_HEIGHT, &camera, flipEnemy);
                    moveEnemy(player, enemies[i], map, mapScale);
                    drawHealthBar(enemies[i], renderer, &camera);
                }
            }
            // Inventory Display
            displayInventory(invDisplay.x, invDisplay.y, invDisplay.width, invDisplay.height, renderer, invDisplay.inventoryTexture, &invDisplay.inventoryRect, inventory, displayInv);
        }
        // Inside your rendering loop or function

        // Enemy animation player.
        enemyAnimationTimer++;
        if (enemyAnimationTimer % ENEMY_FRAME_RATE == 0)
        {
            enemyCurrentFrame = (enemyCurrentFrame + 1) % ENEMY_NUM_FRAMES;
        }
        // Check for collisions between the enemy and the player
        checkCollisionEnemyPlayer(enemies, player, enemyCount, &running);

        // Display the player's health bar and avatar in the top right corner
        displayHpBar(player, renderer, uiTextures);
        displayAvatar(renderer, avatarTexture);
        // Display the score at the top of the screen
        char sCount[256];
        snprintf(sCount, sizeof(sCount), "Score: %d", scoreCount);
        displayText(renderer, fontPath, 50, sCount, screenWidth / 2 - 70, 50, (SDL_Color){255, 227, 0, 255});
        // Update the screen
        if (pause)
        {
            // Set the color to black with 39% opacity (100 is the alpha value for semi-transparency)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

            // Define the rectangle that covers the entire screen
            SDL_Rect pauseRect = {0, 0, screenWidth, screenHeight};

            // Clear the screen to avoid artifacts (Optional, based on your render logic)
            SDL_RenderClear(renderer);

            // Draw the semi-transparent rectangle
            SDL_RenderFillRect(renderer, &pauseRect);

            // Render any text or additional elements on top
            displayText(renderer, fontPath, 50, "Game Paused", screenWidth / 2 - 100, screenHeight / 2, (SDL_Color){255, 255, 255, 255});
        }
        SDL_RenderPresent(renderer);

        // Add a small delay to manage frame rate
        SDL_Delay(8);
    }
    tmx_map_free(map);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Free dynamically allocated memory
    free(player);
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (enemies[i] != NULL)
        {
            free(enemies[i]);
        }
    }

    return 0;
}
