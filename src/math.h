#pragma once

struct vec3
{
    union {
        float x, y, z;
        float values[3];
    };
};

struct matrix4
{
    float values[16];
};

