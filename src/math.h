#pragma once

#include <array>
#include <cmath>
#include <random>
#include <string>

template <int N>
struct Vec
{
    std::array<float, N> arr;
    constexpr Vec() : arr{} {}

    template <typename... Args>
    constexpr Vec(Args... args) : arr{static_cast<float>(args)...} {}

    constexpr Vec<N> operator+(const Vec<N>& other) const
    {
        Vec<N> output;
        for (int i = 0; i < N; i++)
            output.arr[i] = this->arr[i] + other.arr[i];
        return output;
    }

    constexpr Vec<N> &operator+=(const Vec<N> &other)
    {
        for (int i = 0; i < N; i++)
            this->arr[i] += other.arr[i];
        return *this;
    }

    constexpr Vec<N> operator-(const Vec<N>& other) const
    {
        Vec<N>  output;
        for (int i = 0; i < N; i++)
            output.arr[i] = this->arr[i] - other.arr[i];
        return output;
    }

    constexpr Vec<N> &operator-=(const Vec<N> &other)
    {
        for (int i = 0; i < N; i++)
            this->arr[i] -= other.arr[i];
        return *this;
    }

    constexpr Vec<N> operator*(const float n) const
    {
        Vec<N> output;
        for (int i = 0; i < N; i++)
            output.arr[i] = this->arr[i] * n;
        return output;
    }

    constexpr Vec<N> normalize() const
    {
        Vec<N> output;
        float m = 1 / magnitude();
        for (int i = 0; i < N; i++)
            output.arr[i] = this->arr[i] * m;
        return output;
    }

    constexpr float magnitude() const
    {
        float sum = 0;
        for (int i = 0; i < N; i++)
            sum += arr[i] * arr[i];
        return std::sqrt(sum);
    }

    constexpr static float dot(const Vec<N>& a, const Vec<N>& b)
    {
        float sum = 0;
        for (int i = 0; i < N; i++)
            sum += a.arr[i] * b.arr[i];
        return sum;
    }

    constexpr static Vec<3> cross(const Vec<3>& a, const Vec<3>& b)
    {
        return Vec<3>(
            a.arr[1] * b.arr[2] - a.arr[2] * b.arr[1],
            a.arr[2] * b.arr[0] - a.arr[0] * b.arr[2],
            a.arr[0] * b.arr[1] - a.arr[1] * b.arr[0]
        );
    }

    std::string to_string() const
    {
        std::string str = "[";
        for (int i = 0; i < N; i++) {
            str += std::to_string(arr[i]);
            if (i < N - 1) str += ", ";
        }
        return str + "]";
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
        if constexpr (N == M) {
            for (int i = 0; i < N; i++)
                arr[i * M + i] = 1;
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

    std::string to_string() const
    {
        std::string str = "[\n";
        for (int i = 0; i < M; i++) {
            str += "    ";
            for (int j = 0; j < N; j++) {
                str += std::to_string(arr[j * M + i]) + ", ";
            }
            str += "\n";
        }
        return str + "]";
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
    Quaternion(float angle, Vec<3> axis)
    {
        float length = sqrt(pow(axis.arr[0], 2) + pow(axis.arr[1], 2) + pow(axis.arr[2], 2));
        float s = std::sin(angle / 2);
        this->w = std::cos(angle / 2);
        this->x = (axis.arr[0] / length) * s;
        this->y = (axis.arr[1] / length) * s;
        this->z = (axis.arr[2] / length) * s;
    }

    Quaternion operator*(const Quaternion& b) const
    {
        return Quaternion(
            this->w * b.w - this->x * b.x - this->y * b.y - this->z * b.z,
            this->w * b.x + this->x * b.w + this->y * b.z - this->z * b.y,
            this->w * b.y - this->x * b.z + this->y * b.w + this->z * b.x,
            this->w * b.z + this->x * b.y - this->y * b.x + this->z * b.w);
    }

    Vec<3> operator*(const Vec<3> v) const
    {
        Vec<3> u = Vec<3>(this->x, this->y, this->z);
        return u * 2.0f * Vec<3>::dot(u, v)
             + v * (this->w * this->w - Vec<3>::dot(u, u))
             + Vec<3>::cross(u, v) * this->w * 2.0;
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

constexpr float random(float min, float max)
{
    std::default_random_engine engine(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(engine);
}

// Compute the LookAt matrix (a.k.a the view matrix)
constexpr Matrix<4, 4> LookAt(Vec<3> position, Vec<3> target, Vec<3> up)
{
    Vec<3> direction = (position - target).normalize(); // backwards (z axis)
    Vec<3> right = Vec<3>::cross(direction, up).normalize(); // x axis
    Vec<3> actual_up = Vec<3>::cross(right, direction); // y axis
    auto a = -Vec<3>::dot(right, position);
    auto b = -Vec<3>::dot(actual_up, position);
    auto c = -Vec<3>::dot(direction, position);
    return Matrix<4, 4>(
        right.arr[0], actual_up.arr[0], direction.arr[0], 0,
        right.arr[1], actual_up.arr[1], direction.arr[1], 0,
        right.arr[2], actual_up.arr[2], direction.arr[2], 0,
        a,            b,                c,                1
    );
}

// Compute the perspective projection matrix:
// near is the distance to the near plane
// far is the distance to the far plane
// aspect is the frustrum's aspet ratio
// fov is the angle (in radians) of how wide the furstum is
constexpr Matrix<4, 4> PerspectiveProjection(float near, float far, float aspect, float fov)
{
    float t = near * std::tan(fov / 2);
    float b = -t;
    float r = t * aspect;
    float l = -r;
    return Matrix<4, 4>(
        2 * near / (r - l),  0,                   (r + l) / (r - l),              0,
        0,                   2 * near / (t - b),  (t + b) / (t - b),              0,
        0,                   0,                   -(far + near) / (far - near),  -1,
        0,                   0,                   -2 * far * near / (far - near), 0
    );
}
