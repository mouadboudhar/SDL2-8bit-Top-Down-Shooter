#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <tmx.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window's dimensions
#define WIN_WIDTH 1280
#define WIN_HEIGHT 756
// Rectangle's dimensions (enemies, player, etc...)
#define RECT_WIDTH 70
#define RECT_HEIGHT 70
// Map's dimensions
#define MAP_WIDTH 1680
#define MAP_HEIGHT 1680
// Projectile
#define RECT_SIZE 50
#define PROJ_SIZE 25
#define MAX_PROJECTILES 100
// Sprite's dimensions
#define SPRITE_SIZE 8
// Checker's size (for the checkered background)
#define CHECKER_SIZE 64
// Min and max distance to spawn the nemies
#define MIN_DIST 500
// Max amount of enemies
#define MAX_ENEMIES 500
// Animation frames
#define FRAME_RATE 6
#define NUM_FRAMES 2
#define ENEMY_NUM_FRAMES 12
#define ENEMY_FRAME_RATE 2
// dimensions for the items
#define ITEM_WIDTH 50
#define ITEM_HEIGHT 50
// Scaling the item's size
#define MAP_SCALE 5
// Cell size for the map grid
#define GRID_CELL_SIZE 100

typedef enum
{
    WEAPON,
    ARMOR,
    CONSUMABLE
} itemType;

// Enum to define consumable effects
typedef enum
{
    HEALING,
    MANA
} consumableEffect;

// Enum to define weapon types
typedef enum
{
    SWORD,
    BOW,
    AXE,
    STAFF
} weaponType;

// Enum to define armor slots
typedef enum
{
    HELMET,
    CHESTPLATE,
    BOOTS
} armorSlot;

// Structure for Weapon stats
typedef struct
{
    weaponType type;
    int damage;
    int bonusDefense;
    int bonusHealth;
    int bonusAttack;
    int bonusSpeed;
} Weapon;

// Structure for Armor stats
typedef struct
{
    armorSlot slot;
    int bonusDefense;
    int bonusHealth;
    int bonusAttack;
    int bonusSpeed;
} Armor;

// Structure for Consumable stats
typedef struct
{
    consumableEffect effect;
    int value;
} Consumable;

// Union to hold specific item stats
typedef union
{
    Weapon weapon;
    Armor armor;
    Consumable consumable;
} ItemDetails;

// Structure for the Item (linked list)
typedef struct Item
{
    itemType type;
    char itemID[30];
    char itemName[30];
    SDL_Texture *itemSprite;
    int spriteX, spriteY;
    ItemDetails details;
    struct Item *next; // Correctly using Item
} Item;

typedef struct Inventory
{
    unsigned int inventorySize;
    unsigned itemCount;
    Item **inventory; // Array of pointers to Item
} Inventory;

typedef struct invetoryDisplay
{
    float x, y;
    int width, height;
    SDL_Texture *inventoryTexture;
    SDL_Rect inventoryRect;
} inventoryDisplay;

typedef struct inventoryItemDisplay
{
    float x, y;
    int inventorySlot;
    char *itemName;
    SDL_Texture *itemSprite;
    int spriteX, spriteY;
    struct inventoryItemDisplay *next;
} inventoryItemDisplay;

// Hashtable for the items
#define TABLE_SIZE 100
extern Item *table[TABLE_SIZE];

typedef struct ObjectNode
{
    tmx_object *object;
    struct ObjectNode *next;
} ObjectNode;

// Projectiles
typedef struct
{
    float x, y;
    float velX, velY;
    int current_frame;
    Uint32 last_frame_time;
    float angle;
    int active;
} Projectile;

// Canera

typedef struct
{
    int x, y;
    int width, height;
} Camera;

// Rigidbody

typedef struct
{
    int attack;
    int attackBonus;
    int dexterity;
    int dexterityBonus;
    int defense;
    int defenseBonus;
    int speed;
    int speedBonus;
} rigidBodyStats;

typedef struct
{
    float x, y;
    float velX, velY;
    float acceleration;
    float maxSpeed;
    float friction;
    float damage;
    int maxhealth;
    int health;
    rigidBodyStats stats;
    bool state;
} RigidBody;

// Grid cell
typedef struct GridCell
{
    ObjectNode *items; // Linked list of items in this grid cell
} GridCell;

// Gloabal vars
extern SDL_Renderer *renderer;
extern bool godmode;
extern int spawnCooldown;
extern int lastSpawnTime;

/*---------------------------------------------Display functions---------------------------------------------*/

tmx_object *detectPlayerObjectCollision(tmx_map *map, RigidBody *player, const char *itemName);
ObjectNode *findObject(tmx_map *map, const char *layer_name, const char *object_name);
ObjectNode *findAllObject(tmx_map *map, const char *layer_name);
void displayItem(SDL_Renderer *renderer, const char *fontPath, int fontSize, tmx_map *map, SDL_Texture *sprite, RigidBody *player, SDL_Rect *spriteRect, Inventory *inv, Item *item, bool interact, Camera *camera);
int getGridIndex(int position, int cellSize);
void displayItems(SDL_Renderer *renderer, const char *fontPath, int fontSize, tmx_map *map, Item *table[TABLE_SIZE], RigidBody *player, Inventory *inv, bool interact, Camera *camera);

/*---------------------------------------------Enemy functions---------------------------------------------*/

RigidBody *initEnemy(RigidBody *player);
int spawnEnemy(RigidBody *enemies[], RigidBody *player, int *enemyCount, int mapScale, tmx_map *map, Camera *camera, int currentTime);
void moveEnemy(RigidBody *player, RigidBody *enemy, tmx_map *map, int mapScale);
void checkCollisionProjectileEnemy(Projectile *projectile, RigidBody *enemies[], int enemyCount, int *scoreCount);

/*---------------------------------------------Initialization functions---------------------------------------------*/

RigidBody *initBodyPosition(RigidBody *body, float x, float y);
RigidBody *initBodyPhysics(RigidBody *body, float velocityX, float velocityY, float acceleration, float maxSpeed, float friction);
RigidBody *initBodyHpDmgSt(RigidBody *body, float damage, int maxhealth, int health, bool state);
RigidBody *initBody(float x, float y, float velocityX, float velocityY, float acceleration, float maxSpeed, float friction, float damage, int maxhealth, int health, bool state);

/*---------------------------------------------Iventory functions---------------------------------------------*/

Inventory *createInventory(unsigned int inventorySize);
void addItemToInventory(Inventory *inv, Item *item);
void printIventory(Inventory *inv);
inventoryItemDisplay *create_node(float x, float y, int i, SDL_Texture *itemSprite, int spriteX, int spriteY);
inventoryItemDisplay *assign_items_to_slots(Inventory *inv);
void display_items_in_slots(SDL_Renderer *renderer, inventoryItemDisplay *head, float xOff, float yOff);
void displayInventory(float x, float y, int width, int height, SDL_Renderer *renderer, SDL_Texture *sprite, SDL_Rect *spriteRect, Inventory *inv, bool displayInv);

/*---------------------------------------------Item functions---------------------------------------------*/

int generateUniqueID();
unsigned int hash(char *itemID);
void initHashTable(Item *table[TABLE_SIZE]);
void insertItem(Item *table[TABLE_SIZE], Item *item);
Item *findItem(Item *table[TABLE_SIZE], char *itemID);
void printHashTable(Item *table[TABLE_SIZE]);
Item *createWeapon(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, weaponType wType, int damage, int bonusDefense, int bonusHealth, int bonusAttack, int bonusSpeed);
Item *createArmor(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, armorSlot slot, int bonusDefense, int bonusHealth, int bonusAttack, int bonusSpeed);
Item *createConsumable(const char *itemName, SDL_Texture *spriteTexture, int spriteX, int spriteY, consumableEffect effect, int value);
int checkCollisionItems(float playerX, float playerY, float playerWidth, float playerHeight,
                        float itemX, float itemY, float itemWidth, float itemHeight);
Item *findItemByName(Item *table[TABLE_SIZE], char *itemName);

/*---------------------------------------------Map functions---------------------------------------------*/

void *SDL_tex_loader(const char *path);
void set_color(int color);
void draw_image_layer(tmx_image *image);
void draw_polyline(double **points, double x, double y, int pointsc, float scale, float camera_x, float camera_y);
void draw_polygon(double **points, double x, double y, int pointsc, float scale, float camera_x, float camera_y);
void draw_objects(tmx_object_group *objgr, Camera *camera, int mapScale);
void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags);
void draw_layer(tmx_map *map, tmx_layer *layer, Camera *camera, int mapScale);
void draw_all_layers(tmx_map *map, tmx_layer *layers, Camera *camera, int mapScale);
void render_map(tmx_map *map, Camera *camera, int mapScale);
tmx_map *mapLoader(const char *map_path);

/*---------------------------------------------Player functions---------------------------------------------*/

void movePlayer(RigidBody *player, tmx_map *map, int mapScale, int up, int down, int left, int right);
void updateCamera(Camera *camera, RigidBody *player);
void shootProjectile(Projectile *projectiles, RigidBody *player, int mouseX, int mouseY, Camera *camera, Uint32 *lastShotTime, float shootingInterval);
void updateProjectiles(Projectile *projectiles);
void displayProjectile(SDL_Renderer *renderer, Projectile *projectile, SDL_Texture *texture,
                       int spriteWidth, int spriteHeight, Camera *camera, SDL_RendererFlip flip,
                       unsigned int frameDelay, float scaleFactor);
void checkCollisionEnemyPlayer(RigidBody *enemies[], RigidBody *player, int enemyCount, int *quit);
void healPlayer(RigidBody *player, int value);

/*---------------------------------------------Ui functions---------------------------------------------*/

void displayAvatar(SDL_Renderer *renderer, SDL_Texture *texture);
void displayHpBar(RigidBody *player, SDL_Renderer *renderer, SDL_Texture *texture);
SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);
TTF_Font *loadFont(const char *fontPath, int size);
void displayText(SDL_Renderer *renderer, const char *fontPath, int fontSize, const char *text, int x, int y, SDL_Color color);

/*---------------------------------------------Utility functions---------------------------------------------*/

SDL_Texture *loadTexture(const char *path, SDL_Renderer *renderer);
void displayBody(SDL_Renderer *renderer, RigidBody *player, SDL_Texture *texture, SDL_Rect *spriteRect, int width, int height, Camera *camera, SDL_RendererFlip flip);
void renderCheckeredBackground(SDL_Renderer *renderer, Camera *camera);
int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void drawHealthBar(RigidBody *body, SDL_Renderer *renderer, Camera *camera);
int player_collision_detection(RigidBody *player, tmx_map *map, int mapScale);
int enemy_collision_detection(RigidBody *player, tmx_map *map, int mapScale);
int projectile_collision_detection(Projectile *proj, tmx_map *map, int mapScale);

#endif