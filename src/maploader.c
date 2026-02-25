#include "game.h"

void *SDL_tex_loader(const char *path)
{
    return IMG_LoadTexture(renderer, path);
}

void set_color(int color)
{
    tmx_col_bytes col = tmx_col_to_bytes(color);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
}

void draw_image_layer(tmx_image *image)
{
    SDL_Rect dim;
    dim.x = dim.y = 0;

    SDL_Texture *texture = (SDL_Texture *)image->resource_image;
    SDL_QueryTexture(texture, NULL, NULL, &(dim.w), &(dim.h));
    SDL_RenderCopy(renderer, texture, NULL, &dim);
}

void draw_polyline(double **points, double x, double y, int pointsc, float scale, float camera_x, float camera_y)
{
    int i;
    for (i = 1; i < pointsc; i++)
    {
        // Apply scaling and camera adjustment
        SDL_RenderDrawLine(renderer,
                           (x + points[i - 1][0]) * scale - camera_x,
                           (y + points[i - 1][1]) * scale - camera_y,
                           (x + points[i][0]) * scale - camera_x,
                           (y + points[i][1]) * scale - camera_y);
    }
}

void draw_polygon(double **points, double x, double y, int pointsc, float scale, float camera_x, float camera_y)
{
    draw_polyline(points, x, y, pointsc, scale, camera_x, camera_y);

    if (pointsc > 2)
    {
        // Close the polygon by connecting the first and last points
        SDL_RenderDrawLine(renderer,
                           (x + points[0][0]) * scale - camera_x,
                           (y + points[0][1]) * scale - camera_y,
                           (x + points[pointsc - 1][0]) * scale - camera_x,
                           (y + points[pointsc - 1][1]) * scale - camera_y);
    }
}

void draw_objects(tmx_object_group *objgr, Camera *camera, int mapScale)
{
    SDL_Rect rect;
    set_color(objgr->color);
    tmx_object *head = objgr->head;

    while (head)
    {
        if (head->visible)
        {
            if (head->obj_type == OT_SQUARE)
            {
                // Scale and adjust position for camera
                rect.x = (head->x * mapScale) - camera->x;
                rect.y = (head->y * mapScale) - camera->y;
                rect.w = head->width * mapScale;
                rect.h = head->height * mapScale;
                SDL_RenderDrawRect(renderer, &rect);
            }
            else if (head->obj_type == OT_POLYGON)
            {
                draw_polygon(head->content.shape->points, head->x, head->y, head->content.shape->points_len, mapScale, camera->x, camera->y);
            }
            else if (head->obj_type == OT_POLYLINE)
            {
                draw_polyline(head->content.shape->points, head->x, head->y, head->content.shape->points_len, mapScale, camera->x, camera->y);
            }
            else if (head->obj_type == OT_ELLIPSE)
            {
                /* FIXME: no function in SDL2 */
            }
        }
        head = head->next;
    }
}

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags)
{
    if (image == NULL)
    {
        SDL_Log("Error: Texture is NULL in draw_tile.");
        return;
    }
    SDL_Rect src_rect, dest_rect;
    src_rect.x = sx;
    src_rect.y = sy;
    src_rect.w = dest_rect.w = sw;
    src_rect.h = dest_rect.h = sh;
    dest_rect.x = dx;
    dest_rect.y = dy;
    SDL_RenderCopy(renderer, (SDL_Texture *)image, &src_rect, &dest_rect);
}

void draw_layer(tmx_map *map, tmx_layer *layer, Camera *camera, int mapScale)
{
    unsigned long i, j;
    unsigned int gid, x, y, w, h, flags;
    tmx_tileset *ts;
    tmx_image *im;
    void *image;

    for (i = 0; i < map->height; i++)
    {
        for (j = 0; j < map->width; j++)
        {
            gid = (layer->content.gids[(i * map->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (map->tiles[gid] != NULL)
            {
                ts = map->tiles[gid]->tileset;
                im = map->tiles[gid]->image;
                x = map->tiles[gid]->ul_x;
                y = map->tiles[gid]->ul_y;
                w = ts->tile_width;
                h = ts->tile_height;
                if (im)
                {
                    image = im->resource_image;
                }
                else
                {
                    image = ts->image->resource_image;
                }
                flags = (layer->content.gids[(i * map->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;

                unsigned int dx = (j * w * mapScale) - (camera->x);
                unsigned int dy = (i * h * mapScale) - (camera->y);
                unsigned int scaledWidth = w * mapScale;
                unsigned int scaledHeight = h * mapScale;

                SDL_Rect src_rect = {x, y, w, h};
                SDL_Rect dest_rect = {dx, dy, scaledWidth, scaledHeight};
                SDL_RenderCopy(renderer, (SDL_Texture *)image, &src_rect, &dest_rect);
            }
        }
    }
}

void draw_all_layers(tmx_map *map, tmx_layer *layers, Camera *camera, int mapScale)
{
    while (layers)
    {
        if (layers->visible)
        {
            if (layers->type == L_GROUP)
                draw_all_layers(map, layers->content.group_head, camera, mapScale);
            else if (layers->type == L_OBJGR)
                draw_objects(layers->content.objgr, camera, mapScale);
            else if (layers->type == L_IMAGE)
                draw_image_layer(layers->content.image);
            else if (layers->type == L_LAYER)
                draw_layer(map, layers, camera, mapScale);
        }
        layers = layers->next;
    }
}

void render_map(tmx_map *map, Camera *camera, int mapScale)
{
    //renderCheckeredBackground(renderer, camera);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_all_layers(map, map->ly_head, camera, mapScale);
}


tmx_map *mapLoader(const char *map_path)
{
    SDL_Texture *spritesheet_floor = SDL_tex_loader("assets/maps/dungeon/atlas_floor-16x16.png");
    if (spritesheet_floor == NULL)
        SDL_Log("Failed to load spritesheet_floor: %s", SDL_GetError());

    SDL_Texture *spritesheet_walls_high = SDL_tex_loader("assets/maps/dungeon/atlas_walls_high-16x32.png");
    if (spritesheet_walls_high == NULL)
        SDL_Log("Failed to load spritesheet_walls_low: %s", SDL_GetError());

    SDL_Texture *spritesheet_walls_low = SDL_tex_loader("assets/maps/dungeon/atlas_walls_low-16x16.png");
    if (spritesheet_walls_low == NULL)
        SDL_Log("Failed to load spritesheet_walls_low: %s", SDL_GetError());

    SDL_Texture *spritesheet_dungeon_tileset = SDL_tex_loader("assets/maps/dungeon/tileset.png");
    if (spritesheet_dungeon_tileset == NULL)
        SDL_Log("Failed to load spritesheet_dungeon_tileset: %s", SDL_GetError());

    tmx_img_load_func = SDL_tex_loader;
    tmx_img_free_func = (void (*)(void *))SDL_DestroyTexture;
    tmx_map *map = tmx_load(map_path);
    if (map == NULL || map->ly_head == NULL)
    {
        SDL_Log("Map or map layers are NULL");
        exit(1);
    }
    return map;
}