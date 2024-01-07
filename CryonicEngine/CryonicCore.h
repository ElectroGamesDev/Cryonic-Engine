#pragma once

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stddef.h>

void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint);

void Draw3DBillboard(Camera camera, Texture2D texture, Vector3 position, float size, Color tint);