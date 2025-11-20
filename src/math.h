#pragma once

#include <functional>
#include <math.h>

struct Vec2 {
    Vec2() : x(0), y(0) { }
    Vec2(float a, float b) : x(a), y(b) { }
    float x, y;
};

struct Vec3 {
    Vec3() : x(0), y(0), z(0) { }
    Vec3(float a, float b, float c) : x(a), y(b), z(c) { }

    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }

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

    Vec3 operator*(const float v) const { return Vec3(x * v, y * v, z * v); }

    float& operator[](int index) { return index == 0 ? x : index == 1 ? y : z; }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 floor() const { return Vec3(std::floor(x), std::floor(y), std::floor(z)); }

    Vec3 norm() const
    {
        float len = length();
        return Vec3(x / len, y / len, z / len);
    }

    static Vec3 cross(Vec3 a, Vec3 b)
    {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    static float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

    float x, y, z;
};

struct Vec3Hasher {
    std::size_t operator()(const Vec3& v) const
    {
        using fh = std::hash<float>;
        return fh()(fh()(v.x) ^ fh()(v.y) ^ fh()(v.z));
    }
};

struct Quaternion {
    Quaternion() : x(0), y(0), z(0), w(0) { }

    Quaternion(float d, float a, float b, float c) : x(a), y(b), z(c), w(d) { }

    Quaternion operator*(const Quaternion& a)
    {
        return Quaternion(w * a.w - x * a.x - y * a.y - z * a.z,
            w * a.x + x * a.w + y * a.z - z * a.y, w * a.y - x * a.z + y * a.w + z * a.x,
            w * a.z + x * a.y - y * a.x + z * a.w);
    }

    Quaternion norm() const
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

struct Matrix4 {
    Matrix4() // default to an identity matrix
    {
        for (int i = 0; i < 16; i++)
            m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
        m_valid = true;
    }

    Matrix4(bool valid) : m_valid(valid) { }

    Matrix4 operator*(const Matrix4& b) const
    {
        // dot product using column major ordering
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col * 4 + row] = m[0 * 4 + row] * b.m[col * 4 + 0]
                    + m[1 * 4 + row] * b.m[col * 4 + 1]
                    + m[2 * 4 + row] * b.m[col * 4 + 2]
                    + m[3 * 4 + row] * b.m[col * 4 + 3];
            }
        }
        return result;
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
        m.m[0] = 1.0f / (aspect * tan_half);
        m.m[5] = 1.0f / tan_half;
        m.m[10] = -(far + near) / (far - near);
        m.m[11] = -1.0f;
        m.m[14] = -(2.0f * far * near) / (far - near);
        m.m[15] = 0.0f;
        return m;
    }

    // column-major matrix inverse, yes this is a mess
    Matrix4 inverse() const
    {
        Matrix4 out;

        // access helper for column-major: a[col * 4 + row]
        auto at = [&](int col, int row) -> float { return m[col * 4 + row]; };

        auto s0 = at(0, 0) * at(1, 1) - at(1, 0) * at(0, 1);
        auto s1 = at(0, 0) * at(1, 2) - at(1, 0) * at(0, 2);
        auto s2 = at(0, 0) * at(1, 3) - at(1, 0) * at(0, 3);
        auto s3 = at(0, 1) * at(1, 2) - at(1, 1) * at(0, 2);
        auto s4 = at(0, 1) * at(1, 3) - at(1, 1) * at(0, 3);
        auto s5 = at(0, 2) * at(1, 3) - at(1, 2) * at(0, 3);

        auto c5 = at(2, 2) * at(3, 3) - at(3, 2) * at(2, 3);
        auto c4 = at(2, 1) * at(3, 3) - at(3, 1) * at(2, 3);
        auto c3 = at(2, 1) * at(3, 2) - at(3, 1) * at(2, 2);
        auto c2 = at(2, 0) * at(3, 3) - at(3, 0) * at(2, 3);
        auto c1 = at(2, 0) * at(3, 2) - at(3, 0) * at(2, 2);
        auto c0 = at(2, 0) * at(3, 1) - at(3, 0) * at(2, 1);

        auto det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

        if (det == 0.0) {
            out.m_valid = false;
            return out;
        }

        auto invdet = 1.0 / det;

        // column 0
        out.m[0] = (at(1, 1) * c5 - at(1, 2) * c4 + at(1, 3) * c3) * invdet;
        out.m[1] = (-at(1, 0) * c5 + at(1, 2) * c2 - at(1, 3) * c1) * invdet;
        out.m[2] = (at(1, 0) * c4 - at(1, 1) * c2 + at(1, 3) * c0) * invdet;
        out.m[3] = (-at(1, 0) * c3 + at(1, 1) * c1 - at(1, 2) * c0) * invdet;

        // column 1
        out.m[4] = (-at(0, 1) * c5 + at(0, 2) * c4 - at(0, 3) * c3) * invdet;
        out.m[5] = (at(0, 0) * c5 - at(0, 2) * c2 + at(0, 3) * c1) * invdet;
        out.m[6] = (-at(0, 0) * c4 + at(0, 1) * c2 - at(0, 3) * c0) * invdet;
        out.m[7] = (at(0, 0) * c3 - at(0, 1) * c1 + at(0, 2) * c0) * invdet;

        // column 2
        out.m[8] = (at(3, 1) * s5 - at(3, 2) * s4 + at(3, 3) * s3) * invdet;
        out.m[9] = (-at(3, 0) * s5 + at(3, 2) * s2 - at(3, 3) * s1) * invdet;
        out.m[10] = (at(3, 0) * s4 - at(3, 1) * s2 + at(3, 3) * s0) * invdet;
        out.m[11] = (-at(3, 0) * s3 + at(3, 1) * s1 - at(3, 2) * s0) * invdet;

        // column 3
        out.m[12] = (-at(2, 1) * s5 + at(2, 2) * s4 - at(2, 3) * s3) * invdet;
        out.m[13] = (at(2, 0) * s5 - at(2, 2) * s2 + at(2, 3) * s1) * invdet;
        out.m[14] = (-at(2, 0) * s4 + at(2, 1) * s2 - at(2, 3) * s0) * invdet;
        out.m[15] = (at(2, 0) * s3 - at(2, 1) * s1 + at(2, 2) * s0) * invdet;

        return out;
    }

    float m[16];
    bool m_valid;
};
