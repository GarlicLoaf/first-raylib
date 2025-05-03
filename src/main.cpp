#include <cstdlib>
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
    Rectangle hitbox;
    Rectangle shape;
    Vector2 pos;
} Enemy;

void DrawHitboxes(std::list<Enemy> *enemy_queue, Player *player, std::list<Magic> *magic_queue) {
    for (Enemy &enemy : *enemy_queue) {
        DrawRectangle(enemy.hitbox.x, enemy.hitbox.y, enemy.hitbox.width,
                      enemy.hitbox.height, GREEN);
        DrawRectangle(enemy.pos.x, enemy.pos.y, 5.0f, 5.0f, ORANGE);
    }
    DrawRectangle(player->hurtbox.x, player->hurtbox.y, player->hurtbox.width,
                  player->hurtbox.height, BLUE);

    DrawRectangle(player->pos.x, player->pos.y, 5.0f, 5.0f, YELLOW);

    for (Magic &magic : *magic_queue) {
        DrawCircle(magic.circ_pos.x, magic.circ_pos.y, magic.circ_radius, RED);
    }
}

void SpawnEnemy(Vector2 *player_pos, std::list<Enemy> *enemy_queue,
                std::list<Vector2> *spawner_positions) {
    int rand_spawner = std::rand() % 2;
    Vector2 spawner_pos = *next(spawner_positions->begin(), rand_spawner);

    Rectangle skelly_rect{64.0f, 16.0f, tileset_size, tileset_size};
    Enemy skelly{
        0, 1,
        Rectangle{spawner_pos.x * 4, spawner_pos.y * 4, 16.0f * 4, 16.0f * 4},
        skelly_rect,
        Vector2{spawner_pos.x * 4 + 7.0f * 4, spawner_pos.y * 4 + 9.0f * 4}};
    enemy_queue->push_back(skelly);
}

int main() {
    int debug_level{0};
    // initialize the game
    SearchAndSetResourceDir("resources");

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(screen_width, screen_height, "First Prototype");
    SetTargetFPS(29);

    const Texture2D tileset = LoadTexture("tileset.png");
    const Texture2D player_texture = LoadTexture("player.png");
    const Texture2D magic_texture = LoadTexture("flame.png");

    const int map_size{10};
    const int tileset_width{10};
    const float scale{4.0f};
    const Rectangle map_boundary{16.0f * scale + 19.0f, 16.0f * scale + 5.0f,
                                 (map_size - 3) * tileset_size * scale + 10,
                                 (map_size - 3) * tileset_size * scale + 35};

    Map base_map = {{0}, tileset_width, map_size};

    for (int y{0}; y < map_size; y++) {
        for (int x{0}; x < map_size; x++) {
            base_map.tiles[y][x] = 1;
        }
    }

    TileMap(&base_map, map_size, map_size);

    Rectangle player_hurtbox{(screen_width / 2.0f) + 16.0f,
                             (screen_height / 2.0f) + 40.0f, 8.0f * 4.0f,
                             4.0f * 4.0f};
    Player player{2,
                  Vector2{screen_width / 2.0f + 7.0f * 4.0f,
                          screen_height / 2.0f + 9.0f * 4.0f},
                  player_hurtbox};
    std::list<Magic> magic_queue{};
    std::list<Enemy> enemy_queue{};

    int enemy_counter{};
    const int max_enemies{4};
    int total_enemies{0};
    const int to_defeat{1};

    std::list<Vector2> spawner_positions{Vector2{32.0f, 32.0f},
                                         Vector2{112.0f, 112.0f}};

    while (!WindowShouldClose()) {
        PlayerInput(&player, &map_boundary, &magic_queue);

        // magic update step
        for (auto magic{magic_queue.begin()}; magic != magic_queue.end();
             ++magic) {
            magic->pos =
                Vector2Add(magic->pos, Vector2Scale(magic->direction, 8.0f));
            magic->circ_pos =
                Vector2Add(magic->circ_pos, Vector2Scale(magic->direction, 8.0f));
            magic->lifetime -= 5;

            if (magic->lifetime <= 0) {
                magic = magic_queue.erase(magic);
            }

            for (auto enemy{enemy_queue.begin()}; enemy != enemy_queue.end();
                 ++enemy) {
                if (CheckCollisionRecs(enemy->hitbox, magic->shape)) {
                    magic = magic_queue.erase(magic);
                    enemy = enemy_queue.erase(enemy);
                    enemy_counter--;
                }
            }
        }

        // enemy spawning logic
        if (enemy_counter <= 4 && total_enemies < to_defeat) {
            SpawnEnemy(&player.pos, &enemy_queue, &spawner_positions);
            enemy_counter++;
            total_enemies++;
        }

        // enemy movement and collision
        for (Enemy &enemy : enemy_queue) {
            // movement
            const Vector2 move_direction{Vector2Normalize(Vector2Subtract(
                player.pos, Vector2{enemy.hitbox.x, enemy.hitbox.y}))};

            enemy.hitbox.x += move_direction.x;
            enemy.hitbox.y += move_direction.y;
            enemy.pos = Vector2Add(enemy.pos, move_direction);

            // collision
            if (CheckCollisionRecs(player.hurtbox, enemy.hitbox)) {
                Vector2 knock_direction{
                    Vector2Normalize(Vector2Subtract(player.pos, enemy.pos))};
                KnockbackPlayer(&player, &map_boundary, &knock_direction,
                                40.0f);
            }
        }

        if (IsKeyPressed(KEY_C)) {
            debug_level ^= 1;
        }

        // Drawing phase
        BeginDrawing();

        DrawMap(&base_map, &tileset, tileset_size);
        DrawSpawners(&tileset, &spawner_positions);
        DrawPlayer(&player, &player_texture, tileset_size);

        // draw enemy
        for (auto enemy = enemy_queue.begin(); enemy != enemy_queue.end();
             ++enemy) {
            Rectangle target{enemy->hitbox.x, enemy->hitbox.y, tileset_size * 4,
                             tileset_size * 4};
            DrawTexturePro(player_texture, enemy->shape, target,
                           Vector2{0.0f, 0.0f}, 0.0f, WHITE);
        }

        // draw the magic
        for (auto magic = magic_queue.begin(); magic != magic_queue.end();
             ++magic) {
            Rectangle target{magic->pos.x, magic->pos.y,
                             tileset_size * 2, tileset_size * 4};

            DrawTexturePro(
                magic_texture, magic->shape, target, Vector2{target.width / 2.0f, target.height},
                Vector2Angle(Vector2{1.0f, 0.0f}, magic->direction) * RAD2DEG -
                    90.0f,
                WHITE);
        }

        if (debug_level) {
            DrawHitboxes(&enemy_queue, &player, &magic_queue);
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(player_texture);
    UnloadTexture(magic_texture);

    return 0;
}
