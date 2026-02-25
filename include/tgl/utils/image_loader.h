#ifndef TGL_UTILS_IMAGE_LOADER_H
#define TGL_UTILS_IMAGE_LOADER_H

#include "../core/types.h"
#include <string>
#include <vector>

namespace tgl {

struct Image {
    std::vector<u8> pixels;
    int width = 0;
    int height = 0;
    int channels = 0;

    bool valid() const { return !pixels.empty() && width > 0 && height > 0; }
};

// Minimal built-in BMP/TGA loader. For PNG/JPG, link stb_image.
namespace image {

Image load(const std::string& path);
Image load_from_memory(const u8* data, size_t size);
void free(Image& img);

// Built-in TGA loader (no external deps)
Image load_tga(const std::string& path);
Image load_tga_from_memory(const u8* data, size_t size);

// Built-in BMP loader (no external deps)
Image load_bmp(const std::string& path);

} // namespace image
} // namespace tgl

#endif // TGL_UTILS_IMAGE_LOADER_H
