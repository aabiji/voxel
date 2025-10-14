#pragma once

#include <array>
#include <assert.h>
#include <cmath>

namespace math {

template <std::size_t N>
struct Vec
{
    std::array<float, N> arr;

    template <typename... Args>
    Vec(Args... args) : arr{static_cast<float>(args)...} {}

    Vec<N> operator+(const Vec<N>& other) const
    {
        Vec<N> output;
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] + other.arr[i];
        return output;
    }

    Vec<N> operator-(const Vec<N>& other) const
    {
        Vec<N>  output;
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] - other.arr[i];
        return output;
    }

    Vec<N> normalize() const
    {
        Vec<N> output;
        float m = 1 / magnitude();
        for (std::size_t i = 0; i < N; i++)
            output.arr[i] = this->arr[i] * m;
        return output;
    }

    float dot(const Vec<N>& other) const
    {
        float sum = 0;
        for (std::size_t i = 0; i < N; i++)
            sum += this->arr[i] * other.arr[i];
        return sum;
    }

    float magnitude() const
    {
        float sum = 0;
        for (std::size_t i = 0; i < N; i++)
            sum += arr[i] * arr[i];
        return std::sqrt(sum);
    }
};

template <std::size_t N, std::size_t M>
struct Matrix
{
    std::array<float, N * M> arr;

    const float* ptr() { return (const float*)arr.data(); }

    template <typename... Args>
    Matrix(Args... args) : arr{static_cast<float>(args)...} {}

    // Identity matrix by default, if possible
    Matrix() : arr{0}
    {
        if (N == M) {
            for (std::size_t i = 0; i < N; i++)
                arr[i * N + i] = 1;
        }
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
