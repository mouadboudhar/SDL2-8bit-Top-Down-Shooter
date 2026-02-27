# Dungeons And Sorcery

An 8-bit style top-down dungeon crawler built in C with SDL2. Play as a mage navigating dungeon rooms, fighting enemies with projectile attacks, collecting items, and managing your inventory.

> **Note:** This project is unfinished and was made as a learning exercise to explore low-level game development with SDL2 and C.

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
│   └── maps/dungeon/       # Dungeon tilemap (.tmx) and tilesets (test map)
├── lib/                    # Bundled tmx, libxml2, zlib static libraries
├── Makefile
└── .gitignore
```

## Maps

The dungeon map included in `assets/maps/dungeon/` is a **test map** provided for demonstration purposes only. You can create your own custom maps using [Tiled](https://www.mapeditor.org/), a free and open-source 2D level editor.

### Creating a New Map with Tiled

1. **Download and install** [Tiled](https://www.mapeditor.org/).
2. **Create a new map** (`File > New > New Map`):
   - Set **Orientation** to `Orthogonal`.
   - Set **Tile Render Order** to `Right Down`.
   - Set **Tile size** to `16 x 16` pixels (matching the existing tileset).
   - Choose your desired map dimensions (the test map uses `21 x 21` tiles).
   - Make sure **Infinite** is unchecked.
3. **Import a tileset**: Go to `Map > New Tileset...`, select a tileset image (e.g. the included `tileset.png`), and set the tile dimensions to `16 x 16`.
4. **Design your map** by painting tiles onto layers. The game expects the following layer names:
   - `floor` — base ground tiles
   - `walls` — wall/collision tiles
   - `props` — decorative objects and props
5. **Add object layers** for collision zones, spawn points, or interactables as needed.
6. **Save** the map as a `.tmx` file (the default Tiled format) inside `assets/maps/`.

### Integrating Your Map

- The game loads `.tmx` maps using the [tmx](https://github.com/baylej/tmx) C library. Place your `.tmx` file and its associated tileset (`.tsx` and tileset image) in a subdirectory under `assets/maps/`.
- Update the map path in the source code (see `src/maploader.c` and `src/main.c`) to point to your new map file.
- Ensure tileset image paths in the `.tsx` file are relative to the `.tmx` file location.

