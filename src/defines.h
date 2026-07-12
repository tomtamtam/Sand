#pragma once

#include "Renderer.h"

#define WIDTH 200
#define HEIGHT 200

#define LEFT true
#define RIGHT false
struct Pixel
{
    Type type;
    int x, y;
    bool sleep;
};

struct SpawnPoint
{
    int x, y;
    int radius;
    float frequency;
};

enum PaintType
{
    P_OBSTACLE,
    P_SAND,
    P_SPAWN
};
