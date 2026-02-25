#include <tgl/graphics/font.h>
#include <tgl/core/log.h>
#include <tgl/utils/file_utils.h>
#include "gl_loader.h"
#include <cstring>
#include <cmath>

/*
 * Minimal bitmap font loader.
 * For full TTF support, users can integrate stb_truetype.h.
 * This provides a basic ASCII bitmap font generator from a simple format.
 */

namespace tgl {

Font::~Font() { destroy(); }

Result Font::load(const std::string& path, int size_pixels) {
    // Check if this is our simple bitmap font format or a TTF
    auto ext = file::extension(path);
    if (ext == ".ttf" || ext == ".otf") {
        log::warn("TTF/OTF loading requires stb_truetype integration. Using fallback bitmap font.");
    }

    // Generate a basic ASCII bitmap font
    // This creates a simple 8x8 pixel font embedded in the library
    size_ = size_pixels;
    line_height_ = (float)size_pixels;

    // Simple 8x8 font atlas for ASCII 32-126 (95 characters)
    int chars_per_row = 16;
    int char_rows = 6; // 96 / 16 = 6
    int char_w = 8, char_h = 8;
    int atlas_w = chars_per_row * char_w;
    int atlas_h = char_rows * char_h;

    // Create atlas pixels (white on transparent)
    std::vector<u8> pixels(atlas_w * atlas_h * 4, 0);

    // Very minimal built-in pixel font data (just fills rectangles as placeholder)
    // In a real implementation, this would contain actual font bitmap data
    for (int ch = 32; ch < 127; ch++) {
        int idx = ch - 32;
        int cx = (idx % chars_per_row) * char_w;
        int cy = (idx / chars_per_row) * char_h;

        // Create a simple rectangle for each character (placeholder)
        for (int y = 1; y < char_h - 1; y++) {
            for (int x = 1; x < char_w - 1; x++) {
                int px = cx + x;
                int py = cy + y;
                int pi = (py * atlas_w + px) * 4;
                pixels[pi + 0] = 255;
                pixels[pi + 1] = 255;
                pixels[pi + 2] = 255;
                pixels[pi + 3] = 200;
            }
        }

        // Store glyph info
        GlyphInfo g;
        g.uv_min = {(float)cx / atlas_w, (float)cy / atlas_h};
        g.uv_max = {(float)(cx + char_w) / atlas_w, (float)(cy + char_h) / atlas_h};
        g.size = {(float)char_w * size_pixels / 8.0f, (float)char_h * size_pixels / 8.0f};
        g.bearing = {0, g.size.y * 0.8f};
        g.advance = g.size.x;
        glyphs_[(u32)ch] = g;
    }

    // Upload atlas texture
    TextureConfig cfg;
    cfg.min_filter = TextureFilter::Nearest;
    cfg.mag_filter = TextureFilter::Nearest;
    cfg.generate_mipmaps = false;
    auto result = atlas_.create(atlas_w, atlas_h, TextureFormat::RGBA, pixels.data(), cfg);
    if (!result) return result;

    log::info("Font loaded (built-in bitmap, size=%d)", size_pixels);
    return {};
}

void Font::destroy() {
    atlas_.destroy();
    glyphs_.clear();
}

const GlyphInfo* Font::glyph(u32 codepoint) const {
    auto it = glyphs_.find(codepoint);
    return (it != glyphs_.end()) ? &it->second : nullptr;
}

Vec2 Font::measure(const std::string& text) const {
    float x = 0, max_x = 0;
    float y = line_height_;

    for (char c : text) {
        if (c == '\n') {
            max_x = std::max(max_x, x);
            x = 0;
            y += line_height_;
            continue;
        }
        const GlyphInfo* g = glyph((u32)c);
        if (g) x += g->advance;
        else x += size_ * 0.5f;
    }
    max_x = std::max(max_x, x);
    return {max_x, y};
}

} // namespace tgl
