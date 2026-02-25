#ifndef TGL_GRAPHICS_SPRITE_H
#define TGL_GRAPHICS_SPRITE_H

#include "../math/vec2.h"
#include "color.h"
#include "texture.h"

namespace tgl {

struct Rect {
    float x = 0, y = 0, w = 0, h = 0;

    Rect() = default;
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
    }

    bool overlaps(const Rect& r) const {
        return x < r.x + r.w && x + w > r.x && y < r.y + r.h && y + h > r.y;
    }
};

struct Sprite {
    Texture* texture = nullptr;
    Vec2 position = {0, 0};
    Vec2 size = {64, 64};
    Vec2 origin = {0.5f, 0.5f}; // 0-1, center by default
    float rotation = 0.0f;      // radians
    Color tint = Color::white();
    Rect uv_rect = {0, 0, 1, 1}; // normalized UV coordinates
    int layer = 0;                // for sorting
    bool flip_x = false;
    bool flip_y = false;
};

// Sprite sheet / atlas
struct SpriteSheet {
    Texture* texture = nullptr;
    int columns = 1;
    int rows = 1;

    Rect frame_uv(int frame) const {
        int col = frame % columns;
        int row = frame / columns;
        float fw = 1.0f / columns;
        float fh = 1.0f / rows;
        return {col * fw, row * fh, fw, fh};
    }
};

// Animated sprite helper
struct SpriteAnimation {
    SpriteSheet* sheet = nullptr;
    int start_frame = 0;
    int frame_count = 1;
    float frame_duration = 0.1f; // seconds per frame
    bool loop = true;

    float time_ = 0.0f;
    int current_frame_ = 0;

    void update(float dt) {
        time_ += dt;
        if (time_ >= frame_duration) {
            time_ -= frame_duration;
            current_frame_++;
            if (current_frame_ >= frame_count) {
                current_frame_ = loop ? 0 : frame_count - 1;
            }
        }
    }

    Rect current_uv() const {
        return sheet->frame_uv(start_frame + current_frame_);
    }

    void reset() { time_ = 0; current_frame_ = 0; }
};

} // namespace tgl

#endif // TGL_GRAPHICS_SPRITE_H
