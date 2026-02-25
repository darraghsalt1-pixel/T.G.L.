/*
 * T.G.L. Example: FPS Camera
 * Demonstrates a first-person camera with mouse look and WASD movement.
 */
#include <tgl/tgl.h>

static tgl::Camera camera;
static bool mouse_captured = false;

int main() {
    tgl::App app;

    app.on_init = []() {
        camera.position = {0, 2, 5};
        camera.pitch = -10.0f;
        camera.update_vectors();
    };

    app.on_update = [](float dt) {
        auto& input = tgl::App::instance().input();
        auto& window = tgl::App::instance().window();

        if (input.key_pressed(tgl::Key::Escape)) {
            if (mouse_captured) {
                mouse_captured = false;
                window.set_cursor_visible(true);
                window.set_cursor_locked(false);
            } else {
                tgl::App::instance().quit();
            }
        }

        if (input.mouse_pressed(tgl::MouseButton::Left) && !mouse_captured) {
            mouse_captured = true;
            window.set_cursor_visible(false);
            window.set_cursor_locked(true);
        }

        // Mouse look
        if (mouse_captured) {
            tgl::Vec2 delta = input.mouse_delta();
            camera.look(delta.x, -delta.y, 0.15f);
        }

        // WASD + Space/Shift movement
        float speed = 5.0f * dt;
        if (input.key_down(tgl::Key::LeftShift)) speed *= 2.5f;

        if (input.key_down(tgl::Key::W)) camera.move_forward(speed);
        if (input.key_down(tgl::Key::S)) camera.move_backward(speed);
        if (input.key_down(tgl::Key::A)) camera.move_left(speed);
        if (input.key_down(tgl::Key::D)) camera.move_right(speed);
        if (input.key_down(tgl::Key::Space)) camera.move_up(speed);
        if (input.key_down(tgl::Key::LeftControl)) camera.move_down(speed);
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        float aspect = tgl::App::instance().window().aspect();

        r.begin_3d(camera, aspect);

        // Draw a grid floor
        tgl::Mesh floor_mesh = tgl::Mesh::plane(50.0f, 50.0f, 50);
        r.draw_mesh(floor_mesh, r.default_shader(), tgl::Mat4());

        // Draw some objects in the scene
        for (int x = -5; x <= 5; x += 2) {
            for (int z = -5; z <= 5; z += 2) {
                tgl::Mesh cube = tgl::Mesh::cube(0.5f);
                tgl::Mat4 model = tgl::Mat4::translation({(float)x, 0.25f, (float)z});
                r.draw_mesh(cube, r.default_shader(), model);
            }
        }

        // Draw a sphere
        tgl::Mesh sphere = tgl::Mesh::sphere(1.0f);
        tgl::Mat4 sphere_model = tgl::Mat4::translation({0, 1.5f, 0});
        r.draw_mesh(sphere, r.default_shader(), sphere_model);

        // Debug grid lines
        for (int i = -10; i <= 10; i++) {
            r.debug_line({(float)i, 0.01f, -10}, {(float)i, 0.01f, 10}, tgl::Color::gray().with_alpha(0.3f));
            r.debug_line({-10, 0.01f, (float)i}, {10, 0.01f, (float)i}, tgl::Color::gray().with_alpha(0.3f));
        }

        // Axis indicators
        r.debug_line({0,0,0}, {2,0,0}, tgl::Color::red());
        r.debug_line({0,0,0}, {0,2,0}, tgl::Color::green());
        r.debug_line({0,0,0}, {0,0,2}, tgl::Color::blue());

        r.end_3d();
    };

    tgl::AppConfig config;
    config.title = "T.G.L. - FPS Camera (Click to capture mouse, ESC to release)";
    config.width = 1280;
    config.height = 720;
    app.init(config);
    app.run();

    return 0;
}
