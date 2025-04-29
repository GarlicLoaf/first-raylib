#include <iostream>
#include <list>

#include "player/player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "world/map.h"

// define some important constants
constexpr int screen_width{16 * 4 * 10};
constexpr int screen_height{600};
constexpr float tileset_size{16.0f};

typedef struct Enemy {
    int type;
    int health;
    Rectangle shape;
} Enemy;

int main() {
    // initialize the game
    SearchAndSetResourceDir("resources");

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(screen_width, screen_height, "First Prototype");
    SetTargetFPS(29);

    const Texture2D tileset = LoadTexture("tileset.png");
    const Texture2D player_texture = LoadTexture("player.png");

    const int map_size{10};
    const int tileset_width{10};
    const float scale{4.0f};
    const Rectangle map_boundary{16.0f * scale - 6.0f, 16.0f * scale - 30.0f,
                                 (map_size - 3) * tileset_size * scale + 10,
                                 (map_size - 3) * tileset_size * scale + 35};

    Map base_map = {{0}, tileset_width, map_size};

    for (int y{0}; y < map_size; y++) {
        for (int x{0}; x < map_size; x++) {
            base_map.tiles[y][x] = 1;
        }
    }

    TileMap(&base_map, map_size, map_size);

    Player player{screen_width / 2, screen_height / 2};
    std::list<Magic> magic_queue{};
    std::list<Enemy> enemy_queue{};

    Rectangle skelly_rect{64.0f, 16.0f, tileset_size, tileset_size};
    Enemy skelly{0, 1, skelly_rect};
    enemy_queue.push_back(skelly);

    while (!WindowShouldClose()) {
        PlayerInput(&player, &map_boundary, &magic_queue);

        // magic update step
        for (auto magic{magic_queue.begin()}; magic != magic_queue.end();
             ++magic) {
            magic->shape.x += magic->direction.x * 8;
            magic->shape.y += magic->direction.y * 8;
            magic->lifetime -= 5;

            if (magic->lifetime <= 0) {
                magic = magic_queue.erase(magic);
            }

            for (auto enemy{enemy_queue.begin()}; enemy != enemy_queue.end();
                 ++enemy) {
                if (CheckCollisionRecs(enemy->shape, magic->shape)) {
                    magic = magic_queue.erase(magic);
                    enemy = enemy_queue.erase(enemy);
                }
            }
        }

        // Drawing phase
        BeginDrawing();

        DrawMap(&base_map, &tileset, tileset_size);
        DrawPlayer(&player, &player_texture, tileset_size);

        // draw enemy
        for (auto enemy = enemy_queue.begin(); enemy != enemy_queue.end();
             ++enemy) {
            Rectangle target{screen_width / 2, screen_height / 2, tileset_size * 4, tileset_size * 4};
            DrawTexturePro(player_texture, enemy->shape, target, Vector2{0.0f, 0.0f}, 0.0f,
                           WHITE);
        }

        // draw the magic
        for (auto magic = magic_queue.begin(); magic != magic_queue.end();
             ++magic) {
            DrawRectangle(magic->shape.x, magic->shape.y, 10, 10, RED);
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(player_texture);

    return 0;
}
