#include "game.h"

SDL_Texture *loadTexture(const char *path, SDL_Renderer *renderer)
{
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path);

    if (loadedSurface == NULL)
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    else
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

void displayBody(SDL_Renderer *renderer, RigidBody *player, SDL_Texture *texture, SDL_Rect *spriteRect, int width, int height, Camera *camera, SDL_RendererFlip flip)
{
    int strokeThickness = 3;

    // Save the original texture color modulation
    Uint8 r, g, b;
    SDL_GetTextureColorMod(texture, &r, &g, &b); 

    // Set the texture color mod to black for stroke
    SDL_SetTextureColorMod(texture, 0, 0, 0);

    // Draw the stroke by rendering the sprite multiple times with offsets
    SDL_Rect dstRect = {player->x - camera->x, player->y - camera->y, width, height};
    for (int xOffset = -strokeThickness; xOffset <= strokeThickness; ++xOffset)
    {
        for (int yOffset = -strokeThickness; yOffset <= strokeThickness; ++yOffset)
        {
            if (xOffset == 0 && yOffset == 0)
                continue; // Skip the center to avoid drawing the sprite multiple times

            SDL_Rect strokeRect = {dstRect.x + xOffset, dstRect.y + yOffset, dstRect.w, dstRect.h};
            SDL_RenderCopyEx(renderer, texture, spriteRect, &strokeRect, 0.0, NULL, flip);
        }
    }

    // Restore the original texture color modulation
    SDL_SetTextureColorMod(texture, r, g, b);

    // Draw the original sprite on top
    SDL_RenderCopyEx(renderer, texture, spriteRect, &dstRect, 0.0, NULL, flip);
}

void renderCheckeredBackground(SDL_Renderer *renderer, Camera *camera)
{
    for (int y = 0; y < MAP_HEIGHT; y += CHECKER_SIZE)
    {
        for (int x = 0; x < MAP_WIDTH; x += CHECKER_SIZE)
        {
            if (((x / CHECKER_SIZE) % 2) == ((y / CHECKER_SIZE) % 2))
            {
                SDL_SetRenderDrawColor(renderer, 98, 109, 113, 255); // Color 1
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 205, 205, 192, 255); // Color 2
            }
            SDL_Rect rect = {x - camera->x, y - camera->y, CHECKER_SIZE, CHECKER_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}



int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
}

void drawHealthBar(RigidBody *body, SDL_Renderer *renderer, Camera *camera)
{
    float maxHealthWidth = 7.2;
    int hpBarHeight = 8;
    int hpBarWidth = body->health * maxHealthWidth / body->maxhealth*10;

    SDL_Rect fullHpBar = {body->x - camera->x , body->y - camera->y + 80, maxHealthWidth * 10, hpBarHeight};
    SDL_Rect currentHpBar = {body->x - camera->x, body->y - camera->y + 80, hpBarWidth, hpBarHeight};

    SDL_SetRenderDrawColor(renderer, 74, 94, 74, 100);
    SDL_RenderFillRect(renderer, &fullHpBar);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &currentHpBar);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &fullHpBar);
    // Optionally, reset the draw color to default if needed
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

int player_collision_detection(RigidBody *player, tmx_map *map, int mapScale)
{
    tmx_layer *layer = map->ly_head;

    while (layer)
    {
        // Check if the layer is an object group and named "collisions"
        if (layer->type == L_OBJGR && strcmp(layer->name, "collisions") == 0)
        {
            tmx_object *obj = layer->content.objgr->head;
            while (obj)
            {
                float cx = (obj->x * mapScale);
                float cy = (obj->y * mapScale);
                float cw = obj->width * mapScale;
                float ch = obj->height * mapScale;

                if (player->x < cx + cw &&
                    player->x + RECT_WIDTH > cx &&
                    player->y + 60 < cy + ch &&
                    player->y + RECT_HEIGHT > cy)
                {
                    return 1; // Collision detected
                }

                obj = obj->next;
            }
        }
        layer = layer->next;
    }
    return 0;
}

int enemy_collision_detection(RigidBody *player, tmx_map *map, int mapScale)
{
    tmx_layer *layer = map->ly_head;

    while (layer)
    {
        // Check if the layer is an object group and named "collisions"
        if (layer->type == L_OBJGR && strcmp(layer->name, "collisions") == 0)
        {
            tmx_object *obj = layer->content.objgr->head;
            while (obj)
            {
                float cx = (obj->x * mapScale);
                float cy = (obj->y * mapScale);
                float cw = obj->width * mapScale;
                float ch = obj->height * mapScale;

                if (player->x < cx + cw &&
                    player->x + RECT_WIDTH > cx &&
                    player->y + 30 < cy + ch &&
                    player->y + RECT_HEIGHT > cy)
                {
                    return 1; // Collision detected
                }

                obj = obj->next;
            }
        }
        layer = layer->next;
    }
    return 0;
}

int projectile_collision_detection(Projectile* proj, tmx_map *map, int mapScale)
{
    tmx_layer *layer = map->ly_head;

    while (layer)
    {
        // Check if the layer is an object group and named "collisions"
        if (layer->type == L_OBJGR && strcmp(layer->name, "collisions") == 0)
        {
            tmx_object *obj = layer->content.objgr->head;
            while (obj)
            {
                float cx = (obj->x * mapScale);
                float cy = (obj->y * mapScale);
                float cw = obj->width * mapScale;
                float ch = obj->height * mapScale;

                if (proj->x < cx + cw &&
                    proj->x + PROJ_SIZE > cx &&
                    proj->y < cy + ch &&
                    proj->y + PROJ_SIZE > cy)
                {
                    return 1; // Collision detected
                }

                obj = obj->next;
            }
        }
        layer = layer->next;
    }
    return 0;
}