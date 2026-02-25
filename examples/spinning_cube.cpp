/*
 * T.G.L. Example: Spinning Cube
 * Demonstrates 3D rendering with a mesh, camera, and basic lighting.
 */
#include <tgl/tgl.h>

static tgl::Camera camera;
static tgl::Mesh cube;
static float rotation_angle = 0.0f;

int main() {
    tgl::App app;

    app.on_init = []() {
        cube = tgl::Mesh::cube(1.0f);
        camera.position = {0, 1.5f, 4};
        camera.pitch = -15.0f;
        camera.update_vectors();
    };

    app.on_update = [](float dt) {
        auto& input = tgl::App::instance().input();
        if (input.key_pressed(tgl::Key::Escape))
            tgl::App::instance().quit();

        rotation_angle += dt * 45.0f; // 45 degrees per second

        // Simple camera controls
        float speed = 3.0f * dt;
        if (input.key_down(tgl::Key::W)) camera.move_forward(speed);
        if (input.key_down(tgl::Key::S)) camera.move_backward(speed);
        if (input.key_down(tgl::Key::A)) camera.move_left(speed);
        if (input.key_down(tgl::Key::D)) camera.move_right(speed);
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        float aspect = tgl::App::instance().window().aspect();

        r.begin_3d(camera, aspect);

        // Draw spinning cube
        tgl::Mat4 model = tgl::Mat4::rotation_y(tgl::math::radians(rotation_angle))
                        * tgl::Mat4::rotation_x(tgl::math::radians(rotation_angle * 0.7f));
        r.draw_mesh(cube, r.default_shader(), model);

        // Draw floor
        tgl::Mesh floor_mesh = tgl::Mesh::plane(10.0f, 10.0f, 1);
        tgl::Mat4 floor_transform = tgl::Mat4::translation({0, -1, 0});
        r.draw_mesh(floor_mesh, r.default_shader(), floor_transform);

        // Debug visualization
        r.debug_box({0, 0, 0}, {1.1f, 1.1f, 1.1f}, tgl::Color::yellow());
        r.debug_sphere({3, 0, 0}, 0.5f, tgl::Color::cyan());

        r.end_3d();
    };

    tgl::AppConfig config;
    config.title = "T.G.L. - Spinning Cube";
    app.init(config);
    app.run();

    return 0;
}
