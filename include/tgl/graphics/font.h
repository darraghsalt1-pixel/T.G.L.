#ifndef TGL_GRAPHICS_FONT_H
#define TGL_GRAPHICS_FONT_H

#include "../core/types.h"
#include "texture.h"
#include "../math/vec2.h"
#include "color.h"

#include <string>
#include <unordered_map>

namespace tgl {

struct GlyphInfo {
    Vec2 uv_min;
    Vec2 uv_max;
    Vec2 size;
    Vec2 bearing;
    float advance = 0;
};

class Font {
public:
    Font() = default;
    ~Font();

    // Load a TTF/OTF font at a given pixel size
    Result load(const std::string& path, int size_pixels = 32);
    void destroy();

    const GlyphInfo* glyph(u32 codepoint) const;
    const Texture& atlas() const { return atlas_; }
    int size() const { return size_; }
    float line_height() const { return line_height_; }

    // Measure text dimensions
    Vec2 measure(const std::string& text) const;

    bool valid() const { return atlas_.valid(); }

private:
    Texture atlas_;
    std::unordered_map<u32, GlyphInfo> glyphs_;
    int size_ = 0;
    float line_height_ = 0;
};

} // namespace tgl

#endif // TGL_GRAPHICS_FONT_H
