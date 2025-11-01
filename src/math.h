#pragma once

#include <functional>
#include <math.h>

struct Vec3
{
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}

    bool operator==(const Vec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator+(const Vec3& v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3& operator+=(const Vec3& a)
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    Vec3 operator*(const float v) const
    {
        return Vec3(x * v, y * v, z * v);
    }

    Vec3 normalize() const
    {
        float length = std::sqrt(x * x + y * y + z * z);
        return Vec3(x / length, y / length, z / length);
    }

    static Vec3 cross(Vec3 a, Vec3 b)
    {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
    }

    static float dot(Vec3 a, Vec3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
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
    Matrix4() : values{0} {}

    template <typename... T>
    Matrix4(T ...args) : values{args...} {}

    // Compute the projection matrix:
    // near is the distance to the near plane
    // far is the distance to the far plane
    // fov is the field of view of the frustrum in radians
    // aspect is the viewport's aspect ratio
    static Matrix4 projection(float near, float far, float fov, float aspect)
    {
        float tan_half_fov = tan(fov / 2.0f);
        float a = 1.0 / (aspect * tan_half_fov);
        float b = 1.0f / tan_half_fov;
        float c = -(far + near) / (far - near);
        float d = -(2 * far * near) / (far - near);
        // because of opengl, this is in column major ordering
        return Matrix4(
            a,    0.0f,  0.0f, 0.0f,
            0.0f, b,     0.0f, 0.0f,
            0.0f, 0.0f,  c,    d,
            0.0f, 0.0f, -1.0f, 0.0f
        );
    }

    float values[16];
};

struct Quaternion
{
    Quaternion() : x(0), y(0), z(0), w(0) {}

    Quaternion(float d, float a, float b, float c)
        : x(a), y(b), z(c), w(d) {}

    Quaternion operator*(const Quaternion& a)
    {
        return Quaternion(
            w * a.w - x * a.x - y * a.y - z * a.z,
            w * a.x + x * a.w + y * a.z - z * a.y,
            w * a.y - x * a.z + y * a.w + z * a.x,
            w * a.z + x * a.y - y * a.x + z * a.w
        );
    }

    Quaternion normalize() const
    {
        float length = std::sqrt(x * x + y * y + z * z + w * w);
        return Quaternion(w / length, x / length, y / length, z / length);
    }

    Vec3 rotate(Vec3 v)
    {
        Vec3 u(x, y, z); // vector part of quaternion
        float s = w;     // scalar part of quaternion
        return u * 2 * Vec3::dot(u, v)
            + u * (s * s - Vec3::dot(u, u))
            + Vec3::cross(u, v) * 2.0 * s;
    }

    float x, y, z, w;
};

