#ifndef TGL_MATH_UTILS_H
#define TGL_MATH_UTILS_H

#include <cmath>
#include <algorithm>

namespace tgl {
namespace math {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float HALF_PI = PI * 0.5f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;
constexpr float EPSILON = 1e-6f;

inline float radians(float degrees) { return degrees * DEG2RAD; }
inline float degrees(float radians) { return radians * RAD2DEG; }

inline float clamp(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
inline float saturate(float v) { return clamp(v, 0.0f, 1.0f); }
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float inverse_lerp(float a, float b, float v) { return (v - a) / (b - a); }
inline float remap(float v, float in_min, float in_max, float out_min, float out_max) {
    return lerp(out_min, out_max, inverse_lerp(in_min, in_max, v));
}

inline float smoothstep(float edge0, float edge1, float x) {
    float t = saturate((x - edge0) / (edge1 - edge0));
    return t * t * (3.0f - 2.0f * t);
}

inline float sign(float v) { return (v > 0.0f) ? 1.0f : (v < 0.0f) ? -1.0f : 0.0f; }
inline float fract(float v) { return v - std::floor(v); }
inline bool approx(float a, float b, float eps = EPSILON) { return std::abs(a - b) < eps; }

// Simple deterministic random (xorshift32)
inline uint32_t& rng_state() { static uint32_t s = 123456789u; return s; }
inline void seed_random(uint32_t seed) { rng_state() = seed ? seed : 1u; }
inline uint32_t random_u32() {
    uint32_t& s = rng_state();
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return s;
}
inline float random_float() { return static_cast<float>(random_u32()) / 4294967295.0f; }
inline float random_range(float lo, float hi) { return lerp(lo, hi, random_float()); }
inline int random_int(int lo, int hi) { return lo + static_cast<int>(random_u32() % static_cast<uint32_t>(hi - lo + 1)); }

} // namespace math
} // namespace tgl

#endif // TGL_MATH_UTILS_H
