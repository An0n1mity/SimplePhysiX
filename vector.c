#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Vec2 Vec2_set(float x, float y)
{
    Vec2 v = { .x = x, .y = y };
    return v;
}

Vec2 Vec2_add(Vec2 v1, Vec2 v2)
{
    Vec2 v = { .x = v1.x + v2.x, .y = v1.y + v2.y};
    return v;
}

Vec2 Vec2_sub(Vec2 v1, Vec2 v2)
{
    Vec2 v = { .x = v1.x - v2.x, .y = v1.y - v2.y};
    return v;
}

Vec2 Vec2_scale(Vec2 v1, float s)
{
    Vec2 v = {.x = v1.x * s, .y = v1.y * s};
    return v;
}

float Vec2_norm(Vec2 v1)
{
    float n = sqrt(pow(v1.x, 2) + pow(v1.y, 2));
    return n;
}

Vec2 Vec2_normalize(Vec2 v)
{
    Vec2 vn = {v.x/Vec2_norm(v), v.y/Vec2_norm(v)};
    return vn;
}

float Vec2_distance(Vec2 v1, Vec2 v2)
{
    float d = Vec2_norm(Vec2_sub(v1, v2));
    return d;
}
