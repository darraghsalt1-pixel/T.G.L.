#include <tgl/utils/image_loader.h>
#include <tgl/utils/file_utils.h>
#include <tgl/core/log.h>
#include <cstring>
#include <cstdlib>

namespace tgl {
namespace image {

// ---- TGA Loader (no dependencies) ----

Image load_tga(const std::string& path) {
    auto data = file::read_binary(path);
    if (data.empty()) return {};
    return load_tga_from_memory(data.data(), data.size());
}

Image load_tga_from_memory(const u8* data, size_t size) {
    if (size < 18) return {};

    u8 id_length = data[0];
    u8 color_map_type = data[1];
    u8 image_type = data[2];
    int width  = data[12] | (data[13] << 8);
    int height = data[14] | (data[15] << 8);
    u8 bpp = data[16];
    u8 descriptor = data[17];
    (void)color_map_type;

    if (image_type != 2 && image_type != 10) {
        log::error("TGA: unsupported image type %d (only uncompressed/RLE RGB supported)", image_type);
        return {};
    }

    int channels = bpp / 8;
    if (channels < 3 || channels > 4) return {};

    const u8* pixel_data = data + 18 + id_length;
    size_t expected = (size_t)width * height * channels;

    Image img;
    img.width = width;
    img.height = height;
    img.channels = 4; // always output RGBA
    img.pixels.resize(width * height * 4);

    if (image_type == 2) {
        // Uncompressed
        if (18 + id_length + expected > size) return {};
        for (int y = 0; y < height; y++) {
            int src_y = (descriptor & 0x20) ? y : (height - 1 - y);
            for (int x = 0; x < width; x++) {
                int si = (src_y * width + x) * channels;
                int di = (y * width + x) * 4;
                img.pixels[di + 0] = pixel_data[si + 2]; // TGA is BGR
                img.pixels[di + 1] = pixel_data[si + 1];
                img.pixels[di + 2] = pixel_data[si + 0];
                img.pixels[di + 3] = channels == 4 ? pixel_data[si + 3] : 255;
            }
        }
    } else {
        // RLE compressed
        int pixel_count = width * height;
        int current = 0;
        const u8* src = pixel_data;
        std::vector<u8> decoded(pixel_count * channels);
        while (current < pixel_count && (size_t)(src - data) < size) {
            u8 header = *src++;
            int count = (header & 0x7F) + 1;
            if (header & 0x80) {
                // RLE packet
                for (int i = 0; i < count && current < pixel_count; i++, current++) {
                    std::memcpy(&decoded[current * channels], src, channels);
                }
                src += channels;
            } else {
                // Raw packet
                for (int i = 0; i < count && current < pixel_count; i++, current++) {
                    std::memcpy(&decoded[current * channels], src, channels);
                    src += channels;
                }
            }
        }
        // Convert BGR(A) -> RGBA
        for (int y = 0; y < height; y++) {
            int src_y = (descriptor & 0x20) ? y : (height - 1 - y);
            for (int x = 0; x < width; x++) {
                int si = (src_y * width + x) * channels;
                int di = (y * width + x) * 4;
                img.pixels[di + 0] = decoded[si + 2];
                img.pixels[di + 1] = decoded[si + 1];
                img.pixels[di + 2] = decoded[si + 0];
                img.pixels[di + 3] = channels == 4 ? decoded[si + 3] : 255;
            }
        }
    }

    return img;
}

// ---- BMP Loader (no dependencies) ----

Image load_bmp(const std::string& path) {
    auto data = file::read_binary(path);
    if (data.size() < 54) return {};

    if (data[0] != 'B' || data[1] != 'M') return {};

    u32 data_offset;
    i32 width, height;
    u16 bpp;
    std::memcpy(&data_offset, &data[10], 4);
    std::memcpy(&width, &data[18], 4);
    std::memcpy(&height, &data[22], 4);
    std::memcpy(&bpp, &data[28], 2);

    bool flip = height > 0;
    if (height < 0) height = -height;

    int channels = bpp / 8;
    if (channels < 3) {
        log::error("BMP: unsupported bpp %d", bpp);
        return {};
    }

    int row_size = ((width * channels + 3) / 4) * 4; // rows padded to 4 bytes

    Image img;
    img.width = width;
    img.height = height;
    img.channels = 4;
    img.pixels.resize(width * height * 4);

    for (int y = 0; y < height; y++) {
        int src_y = flip ? (height - 1 - y) : y;
        const u8* row = &data[data_offset + src_y * row_size];
        for (int x = 0; x < width; x++) {
            int si = x * channels;
            int di = (y * width + x) * 4;
            img.pixels[di + 0] = row[si + 2]; // BMP is BGR
            img.pixels[di + 1] = row[si + 1];
            img.pixels[di + 2] = row[si + 0];
            img.pixels[di + 3] = channels == 4 ? row[si + 3] : 255;
        }
    }

    return img;
}

// ---- General loader ----

Image load(const std::string& path) {
    auto ext = file::extension(path);
    if (ext == ".tga" || ext == ".TGA") return load_tga(path);
    if (ext == ".bmp" || ext == ".BMP") return load_bmp(path);

    // For PNG/JPG, try stb_image if available
    log::warn("Image format '%s' not natively supported. Use TGA/BMP or integrate stb_image.", ext.c_str());
    return {};
}

Image load_from_memory(const u8* data, size_t size) {
    // Try TGA first (check for valid-looking header)
    if (size > 18) {
        Image img = load_tga_from_memory(data, size);
        if (img.valid()) return img;
    }
    return {};
}

void free(Image& img) {
    img.pixels.clear();
    img.width = img.height = img.channels = 0;
}

} // namespace image
} // namespace tgl
