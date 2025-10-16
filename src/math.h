#pragma once

#include <array>
#include <cmath>

namespace math {

template <std::size_t N>
struct Vec
{
    std::array<float, N> arr;
    constexpr Vec() : arr{0} {}

    template <typename... Args>
    constexpr Vec(Args... args) : arr{static_cast<float>(args)...} {}

    constexpr Vec<N> operator+(const Vec<N>& other) const
    {
        Vec<N> output;
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] + other.arr[i];
        return output;
    }

    constexpr Vec<N> operator-(const Vec<N>& other) const
    {
        Vec<N>  output;
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] - other.arr[i];
        return output;
    }

    constexpr Vec<N> normalize() const
    {
        Vec<N> output;
        float m = 1 / magnitude();
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] * m;
        return output;
    }

    constexpr float dot(const Vec<N>& other) const
    {
        float sum = 0;
        for (std::size_t i = 0; i < N; i++)
            sum += this->arr[i] * other.arr[i];
        return sum;
    }

    constexpr float magnitude() const
    {
        float sum = 0;
        for (std::size_t i = 0; i < N; i++)
            sum += arr[i] * arr[i];
        return std::sqrt(sum);
    }
};

// NOTE: add matrices are stored in column major order, because of OpenGL
template <int N, int M>
struct Matrix
{
    std::array<float, N * M> arr;
    const float* ptr() { return (const float*)arr.data(); }

    template <typename... Args>
    constexpr Matrix(Args... args) : arr{static_cast<float>(args)...} {}

    // Identity matrix by default, if possible
    constexpr Matrix() : arr{0}
    {
        if (N == M) {
            for (int i = 0; i < N; i++)
                arr[i * N + i] = 1;
        }
    }

    // Multiply a NxM matrix by a MxP matrix to get a NxP matrix
    template <int P>
    constexpr Matrix<N, P> operator*(const Matrix<M, P>& other) const
    {
        Matrix<N, P> result;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < P; j++) {
                result.arr[j * N + i] = 0;
                for (int k = 0; k < M; k++) {
                    result.arr[j * N + i] += this->arr[k * N + i] * other.arr[j * M + k];
                }
            }
        }
        return result;
    }

    static Matrix<4, 4> from_translation(float x, float y, float z)
    {
        return Matrix<4, 4>(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        );
    }

    static Matrix<4, 4> from_scale(float x, float y, float z)
    {
        return Matrix<4, 4>(
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        );
    }
};

struct Quaternion
{
    float w; // scalar part
    float x, y, z; // vector part
    Quaternion(float a, float b, float c, float d) : w(a), x(b), y(c), z(d) {}

    // Create a quaternion from a rotation about a set of axis
    // Axis is a list of 3 values, where 1 denotes a rotation about that axis
    // ex: [1, 0, 0] denotes a rotation about the x axis
    // The angle's in radians
    Quaternion(float angle, std::array<float, 3> axis)
    {
        float length = sqrt(pow(axis[0], 2) + pow(axis[1], 2) + pow(axis[2], 2));
        float s = std::sin(angle / 2);
        this->w = std::cos(angle / 2);
        this->x = (axis[0] / length) * s;
        this->y = (axis[1] / length) * s;
        this->z = (axis[2] / length) * s;
    }

    Quaternion operator*(const Quaternion& b) const
    {
        return Quaternion(
            this->w * b.w - this->x * b.x - this->y * b.y - this->z * b.z,
            this->w * b.x + this->x * b.w + this->y * b.z - this->z * b.y,
            this->w * b.y - this->x * b.z + this->y * b.w + this->z * b.x,
            this->w * b.z + this->x * b.y - this->y * b.x + this->z * b.w);
    }

    Matrix<4, 4> to_matrix() const
    {
        using std::pow, std::sqrt;
        float length = sqrt(
            pow(this->w, 2) + pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
        float a = this->x / length, b = this->y / length, c = this->z / length, d = this->w / length;

        // note: this is in column major order for opengl
        return Matrix<4, 4>(
            1 - 2 * (pow(b, 2) + pow(c, 2)), 2 * (a * b + d * c),              2 * (a * c - d * b),             0,
            2 * (a * b - d * c),             1 - 2 * (pow(a, 2) + pow(c, 2)),  2 * (b * c + d * a),             0,
            2 * (a * c + d * b),             2 * (b * c - d * a),              1 - 2 * (pow(a, 2) + pow(b, 2)), 0,
            0,                               0,                                0,                               1
        );
    }
};

constexpr float radians(float degree) { return degree * M_PI / 180; }

constexpr Vec<3> cross(const Vec<3>& a, const Vec<3>& b);

// Compute the LookAt matrix (a.k.a the view matrix)
Matrix<4, 4> LookAt(Vec<3> position, Vec<3> target, Vec<3> up);

// Compute the perspective projection matrix:
// near is the distance to the near plane
// far is the distance to the far plane
// aspect is the frustrum's aspet ratio
// fov is the angle (in radians) of how wide the furstum is
Matrix<4, 4> PerspectiveProjection(float near, float far, float aspect, float fov);

}
