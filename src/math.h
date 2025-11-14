#pragma once

#include <functional>
#include <math.h>

struct Vec2
{
    Vec2() : x(0), y(0) {}
    Vec2(float a, float b) : x(a), y(b) {}
    float x, y;
};

struct Vec3
{
    Vec3() : x(0), y(0), z(0) {}
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

    Vec3 operator-(const Vec3& v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator*(const Vec3& v) const
    {
        return Vec3(x * v.x, y * v.y, z * v.z);
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
    Matrix4() // default to an identity matrix
    {
        for (int i = 0; i < 16; i++) values[i] = 0.0f;
        values[0] = values[5] = values[10] = values[15] = 1.0f;
    }

    Matrix4 operator*(const Matrix4& m) const
    {
        // dot product using column major ordering
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.values[col * 4 + row] =
                    values[0 * 4 + row] * m.values[col * 4 + 0] +
                    values[1 * 4 + row] * m.values[col * 4 + 1] +
                    values[2 * 4 + row] * m.values[col * 4 + 2] +
                    values[3 * 4 + row] * m.values[col * 4 + 3];
            }
        }
        return result;
    }

    static Matrix4 translate(Vec3 offset)
    {
        Matrix4 m;
        m.values[12] = offset.x;
        m.values[13] = offset.y;
        m.values[14] = offset.z;
        return m;
    }

    // Compute the projection matrix:
    // near is the distance to the near plane
    // far is the distance to the far plane
    // fov is the field of view of the frustrum in radians
    // aspect is the viewport's aspect ratio
    static Matrix4 projection(float near, float far, float fov, float aspect)
    {
        float tan_half = tan(fov / 2.0f);
        Matrix4 m;
        m.values[0]  = 1.0f / (aspect * tan_half);
        m.values[5]  = 1.0f / tan_half;
        m.values[10] = -(far + near) / (far - near);
        m.values[11] = -1.0f;
        m.values[14] = -(2.0f * far * near) / (far - near);
        m.values[15] = 0.0f;
        return m;
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

    Vec3 rotate(const Vec3& v) const
    {
        Vec3 qv(x, y, z);
        Vec3 t = Vec3::cross(qv, v) * 2.0f;
        return v + t * w + Vec3::cross(qv, t);
    }

    float x, y, z, w;
};

