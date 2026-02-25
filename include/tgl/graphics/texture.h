#ifndef TGL_GRAPHICS_TEXTURE_H
#define TGL_GRAPHICS_TEXTURE_H

#include "../core/types.h"
#include <string>

namespace tgl {

enum class TextureFilter { Nearest, Linear };
enum class TextureWrap   { Repeat, Clamp, MirrorRepeat };
enum class TextureFormat  { RGB, RGBA, Red, RG };

struct TextureConfig {
    TextureFilter min_filter = TextureFilter::Linear;
    TextureFilter mag_filter = TextureFilter::Linear;
    TextureWrap wrap_s = TextureWrap::Repeat;
    TextureWrap wrap_t = TextureWrap::Repeat;
    bool generate_mipmaps = true;
};

class Texture {
public:
    Texture() = default;
    ~Texture();

    // Load from file (PNG, JPG, BMP, TGA)
    Result load(const std::string& path, const TextureConfig& config = {});
    // Create from raw pixel data
    Result create(int width, int height, TextureFormat format, const u8* data, const TextureConfig& config = {});
    // Create empty texture (for render targets)
    Result create_empty(int width, int height, TextureFormat format, const TextureConfig& config = {});

    void bind(u32 slot = 0) const;
    void unbind() const;
    void destroy();

    int width() const { return width_; }
    int height() const { return height_; }
    u32 id() const { return id_; }
    bool valid() const { return id_ != 0; }

    // 1x1 white texture for default material
    static Texture& white_texture();

private:
    u32 id_ = 0;
    int width_ = 0;
    int height_ = 0;
    TextureFormat format_ = TextureFormat::RGBA;
};

} // namespace tgl

#endif // TGL_GRAPHICS_TEXTURE_H
