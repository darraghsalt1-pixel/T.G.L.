#include <tgl/core/app.h>
#include <tgl/core/log.h>

namespace tgl {

static App* g_instance = nullptr;

App& App::instance() {
    return *g_instance;
}

App::~App() {
    if (g_instance == this) g_instance = nullptr;
}

Result App::init(const AppConfig& config) {
    g_instance = this;

    // Create window
    window_ = make_unique<Window>();
    WindowConfig wc;
    wc.title = config.title;
    wc.width = config.width;
    wc.height = config.height;
    wc.vsync = config.vsync;
    wc.resizable = config.resizable;
    wc.fullscreen = config.fullscreen;
    wc.msaa_samples = config.msaa_samples;
    auto r = window_->create(wc);
    if (!r) return r;

    // Create renderer
    renderer_ = make_unique<Renderer>();
    r = renderer_->init();
    if (!r) return r;

    renderer_->set_viewport(0, 0, config.width, config.height);

    // Create input
    input_ = make_unique<Input>();

    // Wire up window callbacks to input system
    window_->on_key = [this](int key, int /*scancode*/, int action) {
        input_->on_key(key, action);
    };
    window_->on_mouse_move = [this](double x, double y) {
        input_->on_mouse_move(x, y);
    };
    window_->on_mouse_button = [this](int button, int action) {
        input_->on_mouse_button(button, action);
    };
    window_->on_scroll = [this](double x, double y) {
        input_->on_scroll(x, y);
    };
    window_->on_resize = [this](int w, int h) {
        renderer_->set_viewport(0, 0, w, h);
        if (on_resize) on_resize(w, h);
    };

    // Call user init
    if (on_init) on_init();

    log::info("T.G.L. application initialized: %s", config.title.c_str());
    return {};
}

void App::run() {
    running_ = true;
    timer_.reset();

    int frame_count = 0;
    float fps_timer = 0;

    while (running_ && !window_->should_close()) {
        dt_ = timer_.delta();

        // FPS counter
        frame_count++;
        fps_timer += dt_;
        if (fps_timer >= 1.0f) {
            fps_ = frame_count;
            frame_count = 0;
            fps_timer -= 1.0f;
        }

        window_->poll_events();

        if (on_update) on_update(dt_);

        renderer_->begin_frame();
        if (on_render) on_render();
        renderer_->end_frame();

        window_->swap_buffers();
        input_->update();
    }

    if (on_shutdown) on_shutdown();
    renderer_->shutdown();
    window_->destroy();

    log::info("T.G.L. application shut down");
}

void App::quit() {
    running_ = false;
}

} // namespace tgl
