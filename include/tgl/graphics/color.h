#ifndef TGL_GRAPHICS_COLOR_H
#define TGL_GRAPHICS_COLOR_H

#include "../core/types.h"
#include "../math/math_utils.h"

namespace tgl {

struct Color {
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

    Color() = default;
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static Color from_u8(u8 r, u8 g, u8 b, u8 a = 255) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    static Color from_hex(u32 hex) {
        return from_u8((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }

    static Color from_hex_rgb(u32 hex) {
        return from_u8((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF, 255);
    }

    Color lerp(const Color& c, float t) const {
        return {
            math::lerp(r, c.r, t), math::lerp(g, c.g, t),
            math::lerp(b, c.b, t), math::lerp(a, c.a, t)
        };
    }

    Color with_alpha(float alpha) const { return {r, g, b, alpha}; }

    // Predefined colors
    static Color white()       { return {1, 1, 1, 1}; }
    static Color black()       { return {0, 0, 0, 1}; }
    static Color red()         { return {1, 0, 0, 1}; }
    static Color green()       { return {0, 1, 0, 1}; }
    static Color blue()        { return {0, 0, 1, 1}; }
    static Color yellow()      { return {1, 1, 0, 1}; }
    static Color cyan()        { return {0, 1, 1, 1}; }
    static Color magenta()     { return {1, 0, 1, 1}; }
    static Color orange()      { return {1, 0.647f, 0, 1}; }
    static Color gray()        { return {0.5f, 0.5f, 0.5f, 1}; }
    static Color dark_gray()   { return {0.25f, 0.25f, 0.25f, 1}; }
    static Color light_gray()  { return {0.75f, 0.75f, 0.75f, 1}; }
    static Color transparent() { return {0, 0, 0, 0}; }
    static Color cornflower()  { return {0.392f, 0.584f, 0.929f, 1}; }
};

} // namespace tgl

#endif // TGL_GRAPHICS_COLOR_H
