#include "math.h"

namespace math {

constexpr Vec<3> cross(const Vec<3>& a, const Vec<3>& b)
{
    return Vec<3>(
        a.arr[1] * b.arr[2] - a.arr[2] * b.arr[1],
        a.arr[2] * b.arr[0] - a.arr[0] * b.arr[2],
        a.arr[0] * b.arr[1] - a.arr[1] * b.arr[0]
    );
}

Matrix<4, 4> LookAt(Vec<3> position, Vec<3> target, Vec<3> up)
{
    Vec<3> forward = (target - position).normalize();
    Vec<3> right = cross(forward, up).normalize();
    Vec<3> actual_up = cross(right, forward);
    return Matrix<4, 4>(
        right.arr[0],          right.arr[1],          right.arr[2],         -right.dot(position),
        actual_up.arr[0],      actual_up.arr[1],      actual_up.arr[2],     -actual_up.dot(position),
        -forward.arr[0],       -forward.arr[1],       -forward.arr[2],       forward.dot(position),
        0,                     0,                     0,                     1
    );
}

Matrix<4, 4> PerspectiveProjection(float near, float far, float aspect, float fov)
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

}
