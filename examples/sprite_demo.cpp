/*
 * T.G.L. Example: Sprite Demo
 * Demonstrates 2D sprite batching, shapes, and camera.
 */
#include <tgl/tgl.h>
#include <vector>

struct Particle {
    tgl::Vec2 pos;
    tgl::Vec2 vel;
    tgl::Color color;
    float size;
    float life;
};

static tgl::Camera2D camera;
static std::vector<Particle> particles;

static void spawn_particle(const tgl::Vec2& pos) {
    Particle p;
    p.pos = pos;
    float angle = tgl::math::random_range(0, tgl::math::TWO_PI);
    float speed = tgl::math::random_range(50.0f, 200.0f);
    p.vel = {std::cos(angle) * speed, std::sin(angle) * speed};
    p.color = tgl::Color(
        tgl::math::random_float(),
        tgl::math::random_float(),
        tgl::math::random_float(),
        1.0f
    );
    p.size = tgl::math::random_range(4.0f, 16.0f);
    p.life = tgl::math::random_range(1.0f, 3.0f);
    particles.push_back(p);
}

int main() {
    tgl::App app;

    app.on_init = []() {
        tgl::math::seed_random(42);
        // Spawn initial particles
        for (int i = 0; i < 200; i++) {
            spawn_particle({
                tgl::math::random_range(0, 1280.0f),
                tgl::math::random_range(0, 720.0f)
            });
        }
    };

    app.on_update = [](float dt) {
        auto& input = tgl::App::instance().input();
        if (input.key_pressed(tgl::Key::Escape))
            tgl::App::instance().quit();

        // Spawn particles on click
        if (input.mouse_down(tgl::MouseButton::Left)) {
            tgl::Vec2 pos = input.mouse_position();
            for (int i = 0; i < 5; i++) spawn_particle(pos);
        }

        // Camera zoom with scroll
        camera.zoom += input.scroll_delta().y * 0.1f;
        camera.zoom = tgl::math::clamp(camera.zoom, 0.1f, 5.0f);

        // Update particles
        for (auto& p : particles) {
            p.pos += p.vel * dt;
            p.life -= dt;
            p.color.a = tgl::math::saturate(p.life);

            // Bounce off edges
            float w = 1280.0f, h = 720.0f;
            if (p.pos.x < 0 || p.pos.x > w) p.vel.x *= -1;
            if (p.pos.y < 0 || p.pos.y > h) p.vel.y *= -1;
        }

        // Remove dead particles and respawn
        for (auto& p : particles) {
            if (p.life <= 0) {
                p.pos = {tgl::math::random_range(0, 1280.0f), tgl::math::random_range(0, 720.0f)};
                p.life = tgl::math::random_range(1.0f, 3.0f);
                p.color.a = 1.0f;
            }
        }
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        float w = (float)tgl::App::instance().window().width();
        float h = (float)tgl::App::instance().window().height();

        r.begin_2d(camera, w, h);

        // Draw all particles as colored rectangles
        for (auto& p : particles) {
            r.draw_rect(p.pos - tgl::Vec2(p.size * 0.5f), {p.size, p.size}, p.color);
        }

        // Draw some shapes
        r.draw_circle({640, 360}, 50.0f, tgl::Color::cyan().with_alpha(0.3f));
        r.draw_rect_outline({100, 100}, {200, 150}, tgl::Color::yellow(), 2.0f);

        r.end_2d();
    };

    tgl::AppConfig config;
    config.title = "T.G.L. - Sprite & Particle Demo";
    app.init(config);
    app.run();

    return 0;
}
