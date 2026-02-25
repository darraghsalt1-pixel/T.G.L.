#ifndef TGL_CORE_TYPES_H
#define TGL_CORE_TYPES_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

namespace tgl {

// Fixed-width integer types
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

// Smart pointer aliases
template<typename T> using Unique = std::unique_ptr<T>;
template<typename T> using Shared = std::shared_ptr<T>;
template<typename T> using Weak   = std::weak_ptr<T>;

template<typename T, typename... Args>
Unique<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
Shared<T> make_shared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Result type for error handling
enum class ErrorCode {
    None = 0,
    WindowCreationFailed,
    OpenGLInitFailed,
    ShaderCompileFailed,
    ShaderLinkFailed,
    TextureLoadFailed,
    FileNotFound,
    AudioInitFailed,
    InvalidOperation,
};

struct Result {
    ErrorCode code = ErrorCode::None;
    std::string message;

    bool ok() const { return code == ErrorCode::None; }
    operator bool() const { return ok(); }
};

} // namespace tgl

#endif // TGL_CORE_TYPES_H
