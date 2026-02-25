/*
 * T.G.L. Example: Entity Component System
 * Demonstrates the ECS with moving objects and simple physics.
 */
#include <tgl/tgl.h>

// Components
struct Position { tgl::Vec2 pos; };
struct Velocity { tgl::Vec2 vel; };
struct Visual   { tgl::Color color; float size; };
struct Health   { float hp; float max_hp; };

static tgl::World world;

int main() {
    tgl::App app;

    app.on_init = []() {
        tgl::math::seed_random(12345);

        // Create 500 bouncing entities
        for (int i = 0; i < 500; i++) {
            auto e = world.create();

            world.add<Position>(e, {{
                tgl::math::random_range(0, 1280.0f),
                tgl::math::random_range(0, 720.0f)
            }});

            float angle = tgl::math::random_range(0, tgl::math::TWO_PI);
            float speed = tgl::math::random_range(30.0f, 150.0f);
            world.add<Velocity>(e, {{
                std::cos(angle) * speed,
                std::sin(angle) * speed
            }});

            world.add<Visual>(e, {
                tgl::Color(
                    tgl::math::random_float(),
                    tgl::math::random_float(),
                    tgl::math::random_float(), 1.0f),
                tgl::math::random_range(4.0f, 12.0f)
            });

            world.add<Health>(e, {100.0f, 100.0f});
        }
    };

    app.on_update = [](float dt) {
        auto& input = tgl::App::instance().input();
        if (input.key_pressed(tgl::Key::Escape))
            tgl::App::instance().quit();

        // Movement system
        world.each<Position, Velocity>(std::function<void(tgl::Entity, Position&, Velocity&)>(
            [&](tgl::Entity /*e*/, Position& pos, Velocity& vel) {
                pos.pos += vel.vel * dt;

                // Bounce off walls
                if (pos.pos.x < 0 || pos.pos.x > 1280) vel.vel.x *= -1;
                if (pos.pos.y < 0 || pos.pos.y > 720) vel.vel.y *= -1;
                pos.pos.x = tgl::math::clamp(pos.pos.x, 0.0f, 1280.0f);
                pos.pos.y = tgl::math::clamp(pos.pos.y, 0.0f, 720.0f);
            }
        ));
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        float w = (float)tgl::App::instance().window().width();
        float h = (float)tgl::App::instance().window().height();

        r.begin_2d(w, h);

        // Render system
        world.each<Position, Visual>(std::function<void(tgl::Entity, Position&, Visual&)>(
            [&](tgl::Entity /*e*/, Position& pos, Visual& vis) {
                r.draw_rect(
                    pos.pos - tgl::Vec2(vis.size * 0.5f),
                    {vis.size, vis.size},
                    vis.color
                );
            }
        ));

        r.end_2d();
    };

    tgl::AppConfig config;
    config.title = "T.G.L. - ECS Demo (500 entities)";
    app.init(config);
    app.run();

    return 0;
}
