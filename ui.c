#include "game.h"

void displayAvatar(SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDL_Rect srcRect = {0, 0, 38, 38};
    SDL_Rect dstRect = {20, 20, 114, 114};
    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
}

void displayHpBar(RigidBody *player, SDL_Renderer *renderer, SDL_Texture *texture)
{
    int totalFrames = 35;
    int maxHealth = 1000;
    int frameWidth = 40; // Width of each frame in the sprite sheet
    int frameHeight = 9; // Height of each frame in the sprite sheet

    // Calculate health per frame
    float healthPerFrame = (float)maxHealth / totalFrames;
    // Determine the current frame, reverse the order
    int currentHpFrame = (totalFrames - 1) - (int)(player->health / healthPerFrame);

    // Ensure we don't go out of bounds in the sprite sheet
    if (currentHpFrame >= totalFrames)
    {
        currentHpFrame = totalFrames - 1;
    }
    else if (currentHpFrame < 0)
    {
        currentHpFrame = 0;
    }

    SDL_Rect srcRect = {currentHpFrame * frameWidth, 0, frameWidth, frameHeight};
    SDL_Rect dstRect = {140, 50, 200, 45}; // Adjust the destination size if needed

    if (SDL_RenderCopy(renderer, texture, &srcRect, &dstRect) != 0)
    {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
    }
}

SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface)
    {
        fprintf(stderr, "Failed to create text surface: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture)
    {
        fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
    }

    return textTexture;
}

TTF_Font *loadFont(const char *fontPath, int size)
{
    TTF_Font *font = TTF_OpenFont(fontPath, size);
    if (!font)
    {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    }
    return font;
}

void displayText(SDL_Renderer *renderer, const char *fontPath, int fontSize, const char *text, int x, int y, SDL_Color color)
{
    // Outline settings
    SDL_Color outlineColor = {0, 0, 0, 255}; // Black outline color
    int outlineThickness = 2;                // Outline thickness

    // Load the font with the specified size
    TTF_Font *font = loadFont(fontPath, fontSize);
    if (!font)
    {
        return; // Failed to load font, exit the function
    }

    // Set the outline thickness for the font
    TTF_SetFontOutline(font, outlineThickness);

    // Render the outlined text
    SDL_Texture *outlineTexture = renderText(renderer, font, text, outlineColor);
    if (outlineTexture)
    {
        int w, h;
        SDL_QueryTexture(outlineTexture, NULL, NULL, &w, &h);

        // Create a rect for the outline texture
        SDL_Rect outlineRect = {x - outlineThickness, y - outlineThickness, w + outlineThickness, h + outlineThickness};
        SDL_RenderCopy(renderer, outlineTexture, NULL, &outlineRect);
        SDL_DestroyTexture(outlineTexture);
    }

    // Reset the outline thickness to 0 for regular text
    TTF_SetFontOutline(font, 0);

    // Render the regular text
    SDL_Texture *textTexture = renderText(renderer, font, text, color);
    if (textTexture)
    {
        int w, h;
        SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);

        // Create a rect for the text texture
        SDL_Rect textRect = {x, y, w, h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    }

    // Clean up the font
    TTF_CloseFont(font);
}
