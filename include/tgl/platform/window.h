#ifndef TGL_PLATFORM_WINDOW_H
#define TGL_PLATFORM_WINDOW_H

#include "../core/types.h"
#include <string>
#include <functional>

namespace tgl {

struct WindowConfig {
    std::string title = "T.G.L.";
    int width = 1280;
    int height = 720;
    bool vsync = true;
    bool resizable = true;
    bool fullscreen = false;
    int msaa_samples = 4;
};

class Window {
public:
    Window() = default;
    ~Window();

    Result create(const WindowConfig& config);
    void destroy();

    void poll_events();
    void swap_buffers();
    bool should_close() const;
    void set_should_close(bool close);

    int width() const { return width_; }
    int height() const { return height_; }
    float aspect() const { return static_cast<float>(width_) / static_cast<float>(height_); }

    void set_title(const std::string& title);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);
    void set_cursor_visible(bool visible);
    void set_cursor_locked(bool locked);

    void* native_handle() const { return native_handle_; }

    // Callbacks
    std::function<void(int, int)> on_resize;
    std::function<void(int, int, int)> on_key;       // key, scancode, action
    std::function<void(double, double)> on_mouse_move;
    std::function<void(int, int)> on_mouse_button;    // button, action
    std::function<void(double, double)> on_scroll;
    std::function<void(unsigned int)> on_char;

private:
    void* native_handle_ = nullptr;
    void* gl_context_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    bool should_close_ = false;

    // Platform-specific internals
    struct PlatformData;
    PlatformData* platform_ = nullptr;
};

} // namespace tgl

#endif // TGL_PLATFORM_WINDOW_H
