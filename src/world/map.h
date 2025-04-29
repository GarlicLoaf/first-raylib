#ifndef MAP_H
#define MAP_H

#include <vector>

#include "raylib.h"

typedef struct Map {
    int tiles[10][10];  // 2D mapping of tile IDs
    int tileset_width;
    int target_width;
} Map;

void DrawMap(Map *map, const Texture2D *tileset, float tile_size);

void TileMap(Map *map, int height, int width);

#endif
