#ifndef PLAYER_H
#define PLAYER_H

#include <list>

#include "raylib.h"
#include "raymath.h"

struct Player {
    int health;
    Vector2 pos;
    Rectangle hurtbox;
};

typedef struct Magic {
    int lifetime;
    Vector2 pos;
    Vector2 direction;
    Rectangle shape;
} Magic;

const float PLAYER_SPEED = 5.0f;

void PlayerInput(Player *player, const Rectangle *boundary,
                 std::list<Magic> *magic_queue);
void DrawPlayer(Player *player, const Texture2D *texture, float tile_size);
void KnockbackPlayer (Player *player, const Rectangle *boundary, Vector2 *direction, float strength);

#endif
