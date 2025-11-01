#pragma once

struct Vec3
{
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}

    bool operator==(const Vec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    float x, y, z;
};

struct Vec3Hasher
{
    std::size_t operator()(const Vec3& v) const
    {
        using fh = std::hash<float>;
        return fh()(fh()(v.x) ^ fh()(v.y) ^ fh()(v.z));
    }
};

struct Matrix4
{
    float values[16];
};

