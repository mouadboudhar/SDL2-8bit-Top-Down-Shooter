# Dungeons And Sorcery

An 8-bit style top-down dungeon crawler built in C with SDL2. Play as a mage navigating dungeon rooms, fighting enemies with projectile attacks, collecting items, and managing your inventory.

> **Note:** This project is unfinished and was made as a learning exercise to explore low-level game development with SDL2 and C.

## Screenshot

*Coming soon*

## Features

- Top-down dungeon exploration with tile-based maps (Tiled `.tmx` format)
- Projectile-based combat with animated sprite sheets
- Enemy AI that chases the player with physics-based movement
- Inventory system with a hash table item registry
- Item types: weapons, armor, and consumables (health/mana potions)
- Collision detection for walls, enemies, and projectiles
- Camera system that follows the player
- HP bar UI with avatar display
- Pause menu and score tracking
- Godmode for testing (pass any argument to enable)

## Controls

| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move |
| `Left Click` | Shoot projectile |
| `I` | Toggle auto-shoot |
| `E` | Interact / pick up items |
| `B` | Open/close inventory |
| `P` | Pause |
| `ESC` | Quit |

## Dependencies

- [SDL2](https://www.libsdl.org/)
- [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL2_image](https://github.com/libsdl-org/SDL_image)
- [tmx](https://github.com/baylej/tmx) (Tiled map loader)
- libxml2
- zlib

### Install dependencies (Debian/Ubuntu)

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libxml2-dev zlib1g-dev
```

> The `tmx` library is included in `lib/`.

## Building

```bash
make        # Build
make clean  # Remove object files
make fclean # Remove object files and binary
make re     # Full rebuild
```

The compiled binary is output as `game` in the project root.

## Running

```bash
./game          # Normal mode
./game anything # Godmode (invincible + one-shot enemies)
```

## Project Structure

```
├── src/                    # Source files (.c)
│   ├── main.c              # Game loop, input handling, initialization
│   ├── player.c            # Player movement, shooting, collision
│   ├── enemy.c             # Enemy spawning, AI, combat
│   ├── init.c              # RigidBody initialization helpers
│   ├── utils.c             # Texture loading, rendering, collision detection
│   ├── ui.c                # HUD (HP bar, avatar, text rendering)
│   ├── maploader.c         # Tiled .tmx map loading and rendering
│   ├── items.c             # Item creation, hash table management
│   ├── inventory.c         # Inventory management and display
│   └── display.c           # Item display, map object queries
├── include/
│   └── game.h              # Shared header (types, constants, prototypes)
├── assets/
│   ├── sprites/            # Character, UI, projectile, and item sprites
│   ├── fonts/              # Game font
│   └── maps/dungeon/       # Dungeon tilemap (.tmx) and tilesets
├── lib/                    # Bundled tmx, libxml2, zlib static libraries
├── Makefile
└── .gitignore
```

