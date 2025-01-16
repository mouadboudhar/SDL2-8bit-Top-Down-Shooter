#include "game.h"

void movePlayer(RigidBody *player, tmx_map *map, int mapScale, int up, int down, int left, int right)
{
    float accelX = 0.0f;
    float accelY = 0.0f;

    if (up)
        accelY -= player->acceleration;
    if (down)
        accelY += player->acceleration;
    if (left)
        accelX -= player->acceleration;
    if (right)
        accelX += player->acceleration;

    // Normalize diagonal movement
    float accelLength = sqrt(accelX * accelX + accelY * accelY);
    if (accelLength != 0)
    {
        accelX /= accelLength;
        accelY /= accelLength;
    }

    player->velX += accelX * player->acceleration;
    player->velY += accelY * player->acceleration;

    // Apply friction
    player->velX *= player->friction;
    player->velY *= player->friction;

    // Cap speed
    float speed = sqrt(player->velX * player->velX + player->velY * player->velY);
    if (speed > player->maxSpeed)
    {
        player->velX = (player->velX / speed) * player->maxSpeed;
        player->velY = (player->velY / speed) * player->maxSpeed;
    }

    // Save old position before movement
    float old_player_x = player->x;
    float old_player_y = player->y;

    // Move the player
    player->x += player->velX;
    player->y += player->velY;

    if (player_collision_detection(player, map, mapScale) == 1)
    {
        // If collision, revert position and stop movement
        player->x = old_player_x;
        player->y = old_player_y;

        // Optional: Stop movement in the direction of the collision
        player->velX = 0;
        player->velY = 0;
    }

    // Boundaries
    if (player->x < 0)
        player->x = 0;
    if (player->x > MAP_WIDTH - RECT_SIZE)
        player->x = MAP_WIDTH - RECT_SIZE;
    if (player->y < 0)
        player->y = 0;
    if (player->y > MAP_HEIGHT - RECT_SIZE)
        player->y = MAP_HEIGHT - RECT_SIZE;
}

void updateCamera(Camera *camera, RigidBody *player)
{
    //float smoothingFactor = 0.1f;

    float targetX = player->x + RECT_WIDTH / 2 - camera->width / 2;
    float targetY = player->y + RECT_HEIGHT / 2 - camera->height / 2;

    camera->x += (targetX - camera->x);
    camera->y += (targetY - camera->y);

    camera->x = fmax(0, fmin(camera->x, MAP_WIDTH - camera->width));
    camera->y = fmax(0, fmin(camera->y, MAP_HEIGHT - camera->height));
}

void shootProjectile(Projectile *projectiles, RigidBody *player, int mouseX, int mouseY, Camera *camera, Uint32 *lastShotTime, float shootingInterval)
{
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - *lastShotTime >= shootingInterval)
    {
        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (!projectiles[i].active)
            {
                projectiles[i].x = player->x + RECT_SIZE / 2;
                projectiles[i].y = player->y + RECT_SIZE / 2;

                float deltaX = mouseX + camera->x - projectiles[i].x;
                float deltaY = mouseY + camera->y - projectiles[i].y;
                float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
                if (distance != 0)
                {
                    projectiles[i].velX = (deltaX / distance) * 40; // Increased speed
                    projectiles[i].velY = (deltaY / distance) * 40; // Increased speed
                }

                projectiles[i].active = 1;
                projectiles[i].angle = atan2(deltaY, deltaX) * 180 / M_PI;
                projectiles[i].current_frame = 0;
                projectiles[i].last_frame_time = SDL_GetTicks();
                *lastShotTime = currentTime; // Update last shot time
                break;
            }
        }
    }
}

void updateProjectiles(Projectile *projectiles)
{
        projectiles->x += projectiles->velX;
        projectiles->y += projectiles->velY;
}

void displayProjectile(SDL_Renderer *renderer, Projectile *projectile, SDL_Texture *texture,
                       int spriteWidth, int spriteHeight, Camera *camera, SDL_RendererFlip flip,
                       unsigned int frameDelay, float scaleFactor)
{
    int strokeThickness = 1;
    int numFrames = 30; // Total number of frames in your sprite sheet

    if (!projectile->active)
    {
        return; // No need to process inactive projectiles
    }

    // Update the frame based on time
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - projectile->last_frame_time >= frameDelay)
    {
        projectile->current_frame = (projectile->current_frame + 1) % numFrames; // Cycle through frames
        projectile->last_frame_time = currentTime;
    }

    // Define the source rectangle for the current frame
    SDL_Rect spriteRect;
    spriteRect.x = projectile->current_frame * spriteWidth; // x position of the frame in the spritesheet
    spriteRect.y = 0;                                       // Assuming all frames are in a single row
    spriteRect.w = spriteWidth;                             // Width of one frame
    spriteRect.h = spriteHeight;                            // Height of one frame

    // Scale the destination rectangle
    int scaledWidth = (int)(spriteWidth * scaleFactor);
    int scaledHeight = (int)(spriteHeight * scaleFactor);

    // Save the original texture color modulation
    Uint8 r, g, b;
    SDL_GetTextureColorMod(texture, &r, &g, &b);

    // Set the texture color mod to black for stroke
    SDL_SetTextureColorMod(texture, 255, 255, 255);

    // Draw the stroke by rendering the sprite multiple times with offsets
    SDL_Rect dstRect = {projectile->x - camera->x, projectile->y - camera->y, scaledWidth, scaledHeight};
    for (int xOffset = -strokeThickness; xOffset <= strokeThickness; ++xOffset)
    {
        for (int yOffset = -strokeThickness; yOffset <= strokeThickness; ++yOffset)
        {
            if (xOffset == 0 && yOffset == 0)
                continue; // Skip the center to avoid drawing the sprite multiple times

            SDL_Rect strokeRect = {dstRect.x + xOffset, dstRect.y + yOffset, dstRect.w, dstRect.h};
            SDL_RenderCopyEx(renderer, texture, &spriteRect, &strokeRect, projectile->angle, NULL, flip); // Apply stored rotation angle
        }
    }

    // Restore the original texture color modulation
    SDL_SetTextureColorMod(texture, r, g, b);

    // Draw the original sprite on top with the stored rotation
    SDL_RenderCopyEx(renderer, texture, &spriteRect, &dstRect, projectile->angle, NULL, flip); // Apply stored rotation angle
}

void checkCollisionEnemyPlayer(RigidBody *enemies[], RigidBody *player, int enemyCount, int *quit)
{
    for (int i = 0; i < enemyCount; ++i)
    {
        if (enemies[i] != NULL && checkCollision(player->x, player->y, RECT_WIDTH, RECT_HEIGHT, enemies[i]->x, enemies[i]->y, RECT_WIDTH, RECT_HEIGHT))
        {
            if (!godmode)
                player->health -= 10;
            if (player->health <= 0)
            {
                SDL_Log("Game Over!");
                *quit = 0;
            }
        }
    }
}

void healPlayer(RigidBody* player, int value)
{
    player->health += value;
    if(player->health > player->maxhealth)
        player->health = player->maxhealth;
}