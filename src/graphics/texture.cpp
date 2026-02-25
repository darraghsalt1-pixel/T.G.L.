#include <tgl/graphics/texture.h>
#include <tgl/core/log.h>
#include <tgl/utils/image_loader.h>
#include "gl_loader.h"

namespace tgl {

Texture::~Texture() { destroy(); }

static GLenum to_gl_format(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::RGB:  return GL_RGB;
        case TextureFormat::RGBA: return GL_RGBA;
        case TextureFormat::Red:  return GL_RED;
        case TextureFormat::RG:   return GL_RG;
    }
    return GL_RGBA;
}

static GLenum to_gl_filter(TextureFilter f) {
    return f == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR;
}

static GLenum to_gl_wrap(TextureWrap w) {
    switch (w) {
        case TextureWrap::Repeat:       return GL_REPEAT;
        case TextureWrap::Clamp:        return GL_CLAMP_TO_EDGE;
        case TextureWrap::MirrorRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_REPEAT;
}

static void apply_config(const TextureConfig& config) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        config.generate_mipmaps ? (config.min_filter == TextureFilter::Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR)
                                : to_gl_filter(config.min_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl_filter(config.mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl_wrap(config.wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl_wrap(config.wrap_t));
}

Result Texture::load(const std::string& path, const TextureConfig& config) {
    Image img = image::load(path);
    if (!img.valid()) {
        return {ErrorCode::TextureLoadFailed, "Failed to load image: " + path};
    }

    TextureFormat fmt = (img.channels == 4) ? TextureFormat::RGBA :
                        (img.channels == 3) ? TextureFormat::RGB :
                        (img.channels == 2) ? TextureFormat::RG : TextureFormat::Red;

    Result r = create(img.width, img.height, fmt, img.pixels.data(), config);
    return r;
}

Result Texture::create(int width, int height, TextureFormat format, const u8* data, const TextureConfig& config) {
    destroy();
    width_ = width;
    height_ = height;
    format_ = format;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    GLenum gl_fmt = to_gl_format(format);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_fmt, width, height, 0, gl_fmt, GL_UNSIGNED_BYTE, data);

    apply_config(config);
    if (config.generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return {};
}

Result Texture::create_empty(int width, int height, TextureFormat format, const TextureConfig& config) {
    return create(width, height, format, nullptr, config);
}

void Texture::bind(u32 slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::destroy() {
    if (id_) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

Texture& Texture::white_texture() {
    static Texture tex;
    if (!tex.valid()) {
        u8 white[] = {255, 255, 255, 255};
        TextureConfig cfg;
        cfg.generate_mipmaps = false;
        tex.create(1, 1, TextureFormat::RGBA, white, cfg);
    }
    return tex;
}

} // namespace tgl
