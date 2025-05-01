#ifndef MAP_H
#define MAP_H

#include <list>

#include "raylib.h"

typedef struct Map {
    int tiles[10][10];  // 2D mapping of tile IDs
    int tileset_width;
    int target_width;
} Map;

void DrawMap(Map *map, const Texture2D *tileset, float tile_size);
void DrawSpawners(const Texture2D *tileset, std::list<Vector2> *positions);

void TileMap(Map *map, int height, int width);

#endif
