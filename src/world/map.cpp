#include "map.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

void TileMap(Map *map, int height, int width) {
    int directions[8][2] = {
        {-1, 0},  // up
        {1, 0},   // down
        {0, -1},  // left
        {0, 1},   // right
    };

    int out[height][width]{0};

    // bit order: down up right left
    // 0000 - no neighbors
    // 0001 - left
    // 0010 - right
    // 0011 - left + right
    // 0100 - up
    // 0101 - left + up
    // 0110 - right + up
    // 0111 - left + right + up
    // 1000 - down
    // 1001 - left + down
    // 1010 - right + down
    // 1011 - left + right + down
    // 1100 - up + down
    // 1101 - left + up + down
    // 1110 - right + up + down
    // 1111 - all directions

    int tile_to_id[16]{79, 55, 54, 79, 79, 45, 40, 41,
                       79, 5,  0,  1,  79, 15, 10, 23};

    for (int y{0}; y < height; y++) {
        for (int x{0}; x < width; x++) {
            if (map->tiles[y][x] == 1) {
                int mask = 0;
                for (int i{0}; i < 4; i++) {
                    int nx = x + directions[i][0];
                    int ny = y + directions[i][1];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                        map->tiles[ny][nx] >= 1) {
                        // std::cout << ny << " " << nx << " " <<
                        // map->tiles[ny][nx] << "\n";
                        mask |= 1 << i;
                    }
                }
                if (mask == 15) {
                    out[y][x] = 16 + (std::rand() % (19 - 16 + 1));
                } else {
                    out[y][x] = tile_to_id[mask];
                }
            } else {
                out[y][x] = 78;
            }
        }
    }

    for (int y{0}; y < height; y++) {
        for (int x{0}; x < width; x++) {
            map->tiles[y][x] = out[y][x];
        }
    }
}

void DrawMap(Map *map, const Texture2D *tileset, float tile_size) {
    Rectangle rect{0.0f, 0.0f, tile_size, tile_size};

    for (int y{0}; y < map->target_width; y++) {
        for (int x{0}; x < map->target_width; x++) {
            rect.x = (map->tiles[y][x] % map->tileset_width) * tile_size;
            rect.y = (map->tiles[y][x] / map->tileset_width) * tile_size;

            float target_x = x * tile_size * 4;
            float target_y = y * tile_size * 4;

            Rectangle target{target_x, target_y, tile_size * 4, tile_size * 4};

            DrawTexturePro(*tileset, rect, target, Vector2{0.0f, 0.0f}, 0.0f,
                           WHITE);
        }
    }
}

void DrawSpawners(const Texture2D *tileset, std::list<Vector2> *positions) {
    for (const Vector2 &pos : *positions) {
        Rectangle rect{128.0f, 48.0f, 16.0f, 16.0f};
        Rectangle target{pos.x * 4.0f, pos.y * 4.0f, 16.0f * 4.0f,
                         16.0f * 4.0f};
        DrawTexturePro(*tileset, rect, target, Vector2{0.0f, 0.0f}, 0.0f,
                       WHITE);
    }
}
