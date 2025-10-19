#include <random>

#include "math.h"

template struct Vec<3>;
template struct Matrix<4, 4>;

std::default_random_engine engine(std::random_device{}());

float random(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(engine);
}

float radians(float degree) { return degree * M_PI / 180; }

Matrix<4, 4> look_at(Vec<3>& position, Vec<3>& target, Vec<3>& up)
{
    Vec<3> direction = (position - target).normalize(); // backwards (z axis)
    Vec<3> right = Vec<3>::cross(direction, up).normalize(); // x axis
    Vec<3> actual_up = Vec<3>::cross(right, direction); // y axis
    auto a = -Vec<3>::dot(right, position);
    auto b = -Vec<3>::dot(actual_up, position);
    auto c = -Vec<3>::dot(direction, position);
    return Matrix<4, 4>(
        right.x(), actual_up.x(), direction.x(), 0,
        right.y(), actual_up.y(), direction.y(), 0,
        right.z(), actual_up.z(), direction.z(), 0,
        a,         b,             c,             1
    );
}

Matrix<4, 4> perspective_projection(float near, float far, float aspect, float fov)
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
