#ifndef TGL_UTILS_FILE_UTILS_H
#define TGL_UTILS_FILE_UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace tgl {
namespace file {

inline std::string read_text(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

inline std::vector<uint8_t> read_binary(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) return {};
    auto size = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(size);
    f.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

inline bool write_text(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << content;
    return true;
}

inline bool write_binary(const std::string& path, const void* data, size_t size) {
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) return false;
    f.write(static_cast<const char*>(data), size);
    return true;
}

inline bool exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

inline std::string extension(const std::string& path) {
    auto pos = path.rfind('.');
    return (pos != std::string::npos) ? path.substr(pos) : "";
}

inline std::string filename(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}

inline std::string directory(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(0, pos) : ".";
}

} // namespace file
} // namespace tgl

#endif // TGL_UTILS_FILE_UTILS_H
