#ifndef TGL_CORE_APP_H
#define TGL_CORE_APP_H

#include "types.h"
#include "timer.h"
#include "../platform/window.h"
#include "../input/input.h"
#include "../graphics/renderer.h"

#include <string>
#include <functional>

namespace tgl {

struct AppConfig {
    std::string title = "T.G.L. Application";
    int width = 1280;
    int height = 720;
    bool vsync = true;
    bool resizable = true;
    bool fullscreen = false;
    int msaa_samples = 4;
};

class App {
public:
    // Callbacks the user sets
    std::function<void()> on_init;
    std::function<void(float dt)> on_update;
    std::function<void()> on_render;
    std::function<void()> on_shutdown;
    std::function<void(int w, int h)> on_resize;

    App() = default;
    ~App();

    Result init(const AppConfig& config = {});
    void run();
    void quit();

    Window& window() { return *window_; }
    Renderer& renderer() { return *renderer_; }
    Input& input() { return *input_; }

    float delta_time() const { return dt_; }
    float time() const { return timer_.elapsed(); }
    int fps() const { return fps_; }

    static App& instance();

private:
    Unique<Window> window_;
    Unique<Renderer> renderer_;
    Unique<Input> input_;
    Timer timer_;
    float dt_ = 0.0f;
    int fps_ = 0;
    bool running_ = false;
};

} // namespace tgl

#endif // TGL_CORE_APP_H
