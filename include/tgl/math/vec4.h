#ifndef TGL_MATH_VEC4_H
#define TGL_MATH_VEC4_H

#include "vec3.h"
#include <cmath>

namespace tgl {

struct Vec4 {
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

    Vec4() = default;
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    explicit Vec4(float s) : x(s), y(s), z(s), w(s) {}

    Vec4 operator+(const Vec4& v) const { return {x + v.x, y + v.y, z + v.z, w + v.w}; }
    Vec4 operator-(const Vec4& v) const { return {x - v.x, y - v.y, z - v.z, w - v.w}; }
    Vec4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }
    Vec4 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv, w * inv}; }
    Vec4 operator-() const { return {-x, -y, -z, -w}; }

    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vec4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }

    float& operator[](int i) { return (&x)[i]; }
    float operator[](int i) const { return (&x)[i]; }

    float dot(const Vec4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
    float length() const { return std::sqrt(dot(*this)); }
    Vec4 normalized() const { float l = length(); return l > 0 ? *this / l : Vec4{}; }

    Vec3 xyz() const { return {x, y, z}; }
};

inline Vec4 operator*(float s, const Vec4& v) { return v * s; }

} // namespace tgl

#endif // TGL_MATH_VEC4_H
