#ifndef TGL_MATH_VEC3_H
#define TGL_MATH_VEC3_H

#include <cmath>

namespace tgl {

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit Vec3(float s) : x(s), y(s), z(s) {}

    // Arithmetic
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv}; }
    Vec3 operator*(const Vec3& v) const { return {x * v.x, y * v.y, z * v.z}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; return *this; }

    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3& v) const { return !(*this == v); }

    float& operator[](int i) { return (&x)[i]; }
    float operator[](int i) const { return (&x)[i]; }

    // Operations
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float length_sq() const { return x * x + y * y + z * z; }
    Vec3 normalized() const { float l = length(); return l > 0 ? *this / l : Vec3{}; }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3& v) const {
        return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
    }

    float distance(const Vec3& v) const { return (*this - v).length(); }
    Vec3 lerp(const Vec3& v, float t) const { return *this + (v - *this) * t; }
    Vec3 reflect(const Vec3& normal) const { return *this - normal * 2.0f * dot(normal); }

    // Constants
    static Vec3 zero()    { return {0, 0, 0}; }
    static Vec3 one()     { return {1, 1, 1}; }
    static Vec3 up()      { return {0, 1, 0}; }
    static Vec3 down()    { return {0, -1, 0}; }
    static Vec3 left()    { return {-1, 0, 0}; }
    static Vec3 right()   { return {1, 0, 0}; }
    static Vec3 forward() { return {0, 0, -1}; }
    static Vec3 back()    { return {0, 0, 1}; }
};

inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

} // namespace tgl

#endif // TGL_MATH_VEC3_H
