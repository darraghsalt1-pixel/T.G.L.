#ifndef TGL_MATH_VEC2_H
#define TGL_MATH_VEC2_H

#include <cmath>

namespace tgl {

struct Vec2 {
    float x = 0.0f, y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}
    explicit Vec2(float s) : x(s), y(s) {}

    // Arithmetic
    Vec2 operator+(const Vec2& v) const { return {x + v.x, y + v.y}; }
    Vec2 operator-(const Vec2& v) const { return {x - v.x, y - v.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    Vec2 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv}; }
    Vec2 operator*(const Vec2& v) const { return {x * v.x, y * v.y}; }
    Vec2 operator-() const { return {-x, -y}; }

    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; return *this; }

    bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vec2& v) const { return !(*this == v); }

    float& operator[](int i) { return (&x)[i]; }
    float operator[](int i) const { return (&x)[i]; }

    // Operations
    float length() const { return std::sqrt(x * x + y * y); }
    float length_sq() const { return x * x + y * y; }
    Vec2 normalized() const { float l = length(); return l > 0 ? *this / l : Vec2{}; }
    float dot(const Vec2& v) const { return x * v.x + y * v.y; }
    float cross(const Vec2& v) const { return x * v.y - y * v.x; }
    float distance(const Vec2& v) const { return (*this - v).length(); }
    Vec2 lerp(const Vec2& v, float t) const { return *this + (v - *this) * t; }
    Vec2 perpendicular() const { return {-y, x}; }
    Vec2 rotated(float angle) const {
        float c = std::cos(angle), s = std::sin(angle);
        return {x * c - y * s, x * s + y * c};
    }

    float angle() const { return std::atan2(y, x); }
    float angle_to(const Vec2& v) const { return std::atan2(cross(v), dot(v)); }

    // Constants
    static Vec2 zero()  { return {0, 0}; }
    static Vec2 one()   { return {1, 1}; }
    static Vec2 up()    { return {0, 1}; }
    static Vec2 down()  { return {0, -1}; }
    static Vec2 left()  { return {-1, 0}; }
    static Vec2 right() { return {1, 0}; }
};

inline Vec2 operator*(float s, const Vec2& v) { return v * s; }

} // namespace tgl

#endif // TGL_MATH_VEC2_H
