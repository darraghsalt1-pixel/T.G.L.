/*
 * T.G.L. Example: Hello Triangle
 * The simplest possible T.G.L. program - draws a colored triangle.
 */
#include <tgl/tgl.h>

int main() {
    tgl::App app;

    app.on_init = []() {
        tgl::log::info("Hello Triangle initialized!");
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        float w = (float)tgl::App::instance().window().width();
        float h = (float)tgl::App::instance().window().height();

        r.begin_2d(w, h);

        // Draw a colorful triangle using three lines
        tgl::Vec2 center = {w * 0.5f, h * 0.5f};
        float size = 200.0f;

        tgl::Vec2 p1 = center + tgl::Vec2{0, -size};
        tgl::Vec2 p2 = center + tgl::Vec2{-size * 0.866f, size * 0.5f};
        tgl::Vec2 p3 = center + tgl::Vec2{ size * 0.866f, size * 0.5f};

        r.draw_line(p1, p2, tgl::Color::red(), 3.0f);
        r.draw_line(p2, p3, tgl::Color::green(), 3.0f);
        r.draw_line(p3, p1, tgl::Color::blue(), 3.0f);

        // Draw a filled rectangle
        r.draw_rect({50, 50}, {200, 100}, tgl::Color::cyan().with_alpha(0.5f));

        r.end_2d();
    };

    app.on_update = [](float /*dt*/) {
        auto& input = tgl::App::instance().input();
        if (input.key_pressed(tgl::Key::Escape)) {
            tgl::App::instance().quit();
        }
    };

    tgl::AppConfig config;
    config.title = "T.G.L. - Hello Triangle";
    config.width = 800;
    config.height = 600;
    app.init(config);
    app.run();

    return 0;
}
