#include "player.h"
#include "iostream"

void PlayerInput(Player *player, const Rectangle *boundary, std::list<Magic> *magic_queue) {
    Vector2 move_direction{0, 0};
    if (IsKeyDown(KEY_W)) {
        move_direction.y -= PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_S)) {
        move_direction.y += PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_A)) {
        move_direction.x -= PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_D)) {
        move_direction.x += PLAYER_SPEED;
    }

    Vector2 new_pos = Vector2Add(player->pos, move_direction);

    if (new_pos.x > boundary->x && new_pos.x < boundary->x + boundary->width) {
        player->pos.x = new_pos.x;
    }
    if (new_pos.y > boundary->y && new_pos.y < boundary->y + boundary->height) {
        player->pos.y = new_pos.y;
    }


    // handle magic, quick and dirty for now
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos{GetMousePosition()};
        Rectangle rect{player->pos.x, player->pos.y, 10.0f, 10.0f};
        Vector2 direction{Vector2Normalize(Vector2Subtract(mouse_pos, player->pos))};

        Magic new_magic{0, 180, rect, direction};
        magic_queue->push_back(new_magic);
    }
}

void DrawPlayer(Player *player, const Texture2D *texture, float tile_size) {
    Rectangle rect{0.0f, 0.0f, tile_size, tile_size};

    Rectangle target{player->pos.x, player->pos.y, tile_size * 4, tile_size * 4};

    DrawTexturePro(*texture, rect, target, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}
