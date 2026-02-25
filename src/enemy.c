#include "game.h"

int spawnCooldown = 3000; // Time in milliseconds between spawns
int lastSpawnTime = 0;    // Last time an enemy was spawned

RigidBody *initEnemy(RigidBody *player)
{
    RigidBody *enemy = malloc(sizeof(RigidBody));
    enemy = initBodyPhysics(enemy, 0, 0, 0.5f, 6.0f, 0.95f);
    enemy = initBodyHpDmgSt(enemy, 20, 50, 50, true);
    int zone = rand() % 4;
    switch (zone)
    {
    case 0: // Top zone
        enemy->x = rand() % MAP_WIDTH;
        enemy->y = rand() % ((int)player->y - MIN_DIST);
        break;
    case 1: // Bottom zone
        enemy->x = rand() % MAP_WIDTH;
        enemy->y = player->y + MIN_DIST + rand() % (MAP_HEIGHT - (int)player->y - MIN_DIST);
        break;
    case 2: // Left zone
        enemy->x = rand() % ((int)player->x - MIN_DIST);
        enemy->y = rand() % MAP_HEIGHT;
        break;
    case 3: // Right zone
        enemy->x = player->x + MIN_DIST + rand() % (MAP_WIDTH - (int)player->x - MIN_DIST);
        enemy->y = rand() % MAP_HEIGHT;
        break;
    }
    return enemy;
}

int spawnEnemy(RigidBody *enemies[], RigidBody *player, int *enemyCount, int mapScale, tmx_map *map, Camera *camera, int currentTime)
{
    // Check if enough time has passed since the last spawn
    if (currentTime - lastSpawnTime < spawnCooldown)
    {
        return 0; // Cooldown active, don't spawn
    }

    if (*enemyCount < MAX_ENEMIES)
    {
        int spawnAttempts = 0;
        int success = 0;

        while (spawnAttempts < 100 && success == 0)
        {                                // Attempt up to 100 times
            int randomNum = rand() % 50; // Random chance to spawn
            if (randomNum < 1)
            {
                int x = player->x + (rand() % 200 - 100) * MIN_DIST / 100;
                int y = player->y + (rand() % 200 - 100) * MIN_DIST / 100;

                // Ensure the new enemy position is within bounds
                if (x < 0)
                    x = 0;
                else if (x >= MAP_WIDTH)
                    x = MAP_WIDTH - 1;

                if (y < 0)
                    y = 0;
                else if (y >= MAP_HEIGHT)
                    y = MAP_HEIGHT - 1;

                // Check if the new position collides with any map objects
                RigidBody temp_enemy = {.x = x, .y = y};
                if (enemy_collision_detection(&temp_enemy, map, mapScale) == 0)
                {
                    // No collision, spawn enemy
                    enemies[*enemyCount] = initBody(x, y, 0, 0, 2.0f, 4.5f, 0.65f, 30, 100, 100, true);
                    (*enemyCount)++;
                    success = 1; // Mark the spawn as successful
                }
            }
            spawnAttempts++;
        }

        if (success)
        {
            lastSpawnTime = currentTime; // Update last spawn time if successful
        }

        return success; // Return 1 if enemy successfully spawned, 0 if not
    }

    return 0; // No enemy spawned if enemy count is full
}

void moveEnemy(RigidBody *player, RigidBody *enemy, tmx_map *map, int mapScale)
{
    float deltaX = player->x - enemy->x;
    float deltaY = player->y - enemy->y;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (distance != 0)
    {
        enemy->velX += (deltaX / distance) * enemy->acceleration;
        enemy->velY += (deltaY / distance) * enemy->acceleration;
    }

    if (enemy->velX > enemy->maxSpeed)
        enemy->velX = enemy->maxSpeed;

    if (enemy->velX < -enemy->maxSpeed)
        enemy->velX = -enemy->maxSpeed;

    if (enemy->velY > enemy->maxSpeed)
        enemy->velY = enemy->maxSpeed;

    if (enemy->velY < -enemy->maxSpeed)
        enemy->velY = -enemy->maxSpeed;

    enemy->velX *= enemy->friction;
    enemy->velY *= enemy->friction;

    float old_enemy_x = enemy->x;
    float old_enemy_y = enemy->y;

    enemy->x += (int)enemy->velX;
    enemy->y += (int)enemy->velY;

    if (enemy_collision_detection(enemy, map, mapScale) == 1)
    {
        // If collision, revert position and stop movement
        enemy->x = old_enemy_x;
        enemy->y = old_enemy_y;

        // Optional: Stop movement in the direction of the collision
        enemy->velX = 0;
        enemy->velY = 0;
    }
}

void checkCollisionProjectileEnemy(Projectile *projectile, RigidBody *enemies[], int enemyCount, int *scoreCount)
{
    if (projectile->active)
    {
        for (int i = 0; i < enemyCount; ++i)
        {
            if (enemies[i] != NULL && checkCollision(projectile->x, projectile->y, PROJ_SIZE, PROJ_SIZE, enemies[i]->x, enemies[i]->y, RECT_WIDTH, RECT_HEIGHT))
            {
                projectile->active = 0;
                if (godmode)
                    enemies[i]->health -= 10000;
                else
                    enemies[i]->health -= 30;
                printf("enemy shot\n");
                if (enemies[i]->health <= 0)
                {
                    (*scoreCount)++;
                    free(enemies[i]);
                    enemies[i] = NULL;
                }
                break;
            }
        }
    }
}