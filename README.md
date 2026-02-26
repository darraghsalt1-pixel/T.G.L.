# T.G.L. — The Graphics Library

A lightweight, easy-to-use C++17 graphics library built for games and simulations. No bloat, no massive dependency trees — just include and go.

## Features

- **Zero external dependencies** — built-in OpenGL loader, image loaders (TGA/BMP), font rendering
- **Cross-platform** — Windows (Win32/WGL), Linux (X11/GLX), macOS ready
- **Single include** — `#include <tgl/tgl.h>` gets you everything
- **2D & 3D rendering** — auto-batched sprite renderer, 3D mesh pipeline with lighting
- **Built-in primitives** — cube, sphere, plane, cylinder, quad generators
- **Math library** — Vec2/3/4, Mat4, Transform with all common operations
- **Input system** — keyboard, mouse with pressed/released/held state tracking
- **Entity Component System** — lightweight ECS for game object management
- **Camera system** — 3D FPS camera and 2D camera with zoom/rotation
- **Debug drawing** — lines, boxes, spheres for visualization
- **Framebuffers** — render-to-texture support
- **Audio** — WAV loading and playback
- **Font rendering** — built-in bitmap font, TTF ready via stb_truetype

---

## Installation

### Prerequisites

You need a C++17 compiler, CMake 3.16+, and OpenGL 3.3+ drivers.

**Linux (Ubuntu/Debian):**
```bash
sudo apt install build-essential cmake libx11-dev libgl-dev
```

**Linux (Fedora):**
```bash
sudo dnf install gcc-c++ cmake libX11-devel mesa-libGL-devel
```

**Linux (Arch):**
```bash
sudo pacman -S base-devel cmake libx11 mesa
```

**Windows:**
- Install [Visual Studio](https://visualstudio.microsoft.com/) (2019+ with C++ workload) or MinGW-w64
- Install [CMake](https://cmake.org/download/)
- OpenGL drivers come with your GPU driver

**macOS:**
```bash
xcode-select --install
brew install cmake
```

### Build & Install

```bash
# Clone the repo
git clone https://github.com/darraghsalt1-pixel/T.G.L.git
cd T.G.L

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)       # Linux/macOS
# cmake --build . -j   # Windows (or open .sln in Visual Studio)

# (Optional) Install system-wide
sudo make install
```

### CMake Options

| Option | Default | Description |
|---|---|---|
| `TGL_BUILD_EXAMPLES` | `ON` | Build the example programs |
| `TGL_BUILD_TESTS` | `OFF` | Build tests |
| `TGL_USE_WAYLAND` | `OFF` | Use Wayland instead of X11 on Linux |

Example with options:
```bash
cmake .. -DTGL_BUILD_EXAMPLES=ON -DTGL_USE_WAYLAND=OFF
```

### Using T.G.L. in Your Project

**Option A — CMake subdirectory (recommended):**

Put the T.G.L. folder inside your project, then in your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_game)

add_subdirectory(T.G.L.)

add_executable(my_game main.cpp)
target_link_libraries(my_game tgl)
```

**Option B — System install:**

After running `sudo make install`, use `find_package`:

```cmake
find_package(TGL REQUIRED)
add_executable(my_game main.cpp)
target_link_libraries(my_game TGL::tgl)
```

**Option C — Copy headers + static lib:**

Copy `include/tgl/` to your include path and link against `libtgl.a` (or `tgl.lib` on Windows).

---

## Quick Start

### Minimal Window

```cpp
#include <tgl/tgl.h>

int main() {
    tgl::App app;

    app.on_update = [](float dt) {
        if (tgl::App::instance().input().key_pressed(tgl::Key::Escape))
            tgl::App::instance().quit();
    };

    app.init({"Hello T.G.L.", 1280, 720});
    app.run();
}
```

### Drawing 2D Shapes

```cpp
app.on_render = []() {
    auto& r = tgl::App::instance().renderer();
    r.begin_2d(1280, 720);

    r.draw_rect({100, 100}, {200, 150}, tgl::Color::cyan());
    r.draw_circle({640, 360}, 80, tgl::Color::red());
    r.draw_line({0, 0}, {1280, 720}, tgl::Color::yellow(), 2.0f);
    r.draw_rect_outline({300, 200}, {100, 100}, tgl::Color::green(), 2.0f);

    r.end_2d();
};
```

### Drawing 3D Meshes

```cpp
tgl::Camera camera;
tgl::Mesh cube;

app.on_init = []() {
    cube = tgl::Mesh::cube(1.0f);
    camera.position = {0, 2, 5};
    camera.pitch = -15.0f;
    camera.update_vectors();
};

app.on_render = []() {
    auto& r = tgl::App::instance().renderer();

    r.begin_3d(camera, tgl::App::instance().window().aspect());

    tgl::Mat4 model = tgl::Mat4::translation({0, 0.5f, 0})
                    * tgl::Mat4::rotation_y(tgl::App::instance().time());
    r.draw_mesh(cube, r.default_shader(), model);

    r.end_3d();
};
```

### Input Handling

```cpp
app.on_update = [](float dt) {
    auto& input = tgl::App::instance().input();

    // Keyboard
    if (input.key_pressed(tgl::Key::Space))   { /* just pressed */ }
    if (input.key_down(tgl::Key::W))           { /* held down */ }
    if (input.key_released(tgl::Key::LeftShift)) { /* just released */ }

    // Mouse
    if (input.mouse_pressed(tgl::MouseButton::Left)) { /* click */ }
    tgl::Vec2 pos   = input.mouse_position();
    tgl::Vec2 delta = input.mouse_delta();
    tgl::Vec2 scroll = input.scroll_delta();
};
```

### Using the ECS

```cpp
// Define components — any struct works
struct Position { tgl::Vec2 pos; };
struct Velocity { tgl::Vec2 vel; };
struct Visual   { tgl::Color color; float size; };

tgl::World world;

// Create entities
auto e = world.create();
world.add<Position>(e, {{100, 200}});
world.add<Velocity>(e, {{50, -30}});
world.add<Visual>(e, {tgl::Color::red(), 10.0f});

// Systems — iterate entities with specific components
app.on_update = [](float dt) {
    // Movement system
    world.each<Position, Velocity>(
        std::function<void(tgl::Entity, Position&, Velocity&)>(
            [&](tgl::Entity e, Position& pos, Velocity& vel) {
                pos.pos += vel.vel * dt;
            }
        )
    );
};

app.on_render = []() {
    auto& r = tgl::App::instance().renderer();
    r.begin_2d(1280, 720);

    // Render system
    world.each<Position, Visual>(
        std::function<void(tgl::Entity, Position&, Visual&)>(
            [&](tgl::Entity e, Position& pos, Visual& vis) {
                r.draw_rect(pos.pos, {vis.size, vis.size}, vis.color);
            }
        )
    );

    r.end_2d();
};
```

### Sprites & Animation

```cpp
tgl::Texture tex;
tgl::SpriteSheet sheet;

app.on_init = []() {
    tex.load("player.tga");
    sheet.texture = &tex;
    sheet.columns = 4;
    sheet.rows = 2;
};

app.on_render = []() {
    auto& r = tgl::App::instance().renderer();
    r.begin_2d(1280, 720);

    tgl::Sprite sprite;
    sprite.texture = &tex;
    sprite.position = {400, 300};
    sprite.size = {64, 64};
    sprite.uv_rect = sheet.frame_uv(current_frame);
    sprite.tint = tgl::Color::white();
    r.draw_sprite(sprite);

    r.end_2d();
};
```

### FPS Camera with Mouse Look

```cpp
tgl::Camera camera;

app.on_update = [](float dt) {
    auto& input = tgl::App::instance().input();

    // Mouse look
    tgl::Vec2 delta = input.mouse_delta();
    camera.look(delta.x, -delta.y, 0.15f);

    // WASD movement
    float speed = 5.0f * dt;
    if (input.key_down(tgl::Key::W)) camera.move_forward(speed);
    if (input.key_down(tgl::Key::S)) camera.move_backward(speed);
    if (input.key_down(tgl::Key::A)) camera.move_left(speed);
    if (input.key_down(tgl::Key::D)) camera.move_right(speed);
    if (input.key_down(tgl::Key::Space)) camera.move_up(speed);
};
```

### Math Utilities

```cpp
// Vectors
tgl::Vec3 a = {1, 2, 3};
tgl::Vec3 b = {4, 5, 6};
tgl::Vec3 c = a.cross(b);
float d = a.dot(b);
tgl::Vec3 n = a.normalized();
float dist = a.distance(b);
tgl::Vec3 mid = a.lerp(b, 0.5f);

// Matrices
tgl::Mat4 model = tgl::Mat4::translation({1, 0, 0})
                * tgl::Mat4::rotation_y(tgl::math::radians(45.0f))
                * tgl::Mat4::scale(2.0f);

tgl::Mat4 view = tgl::Mat4::look_at(eye, target, up);
tgl::Mat4 proj = tgl::Mat4::perspective(tgl::math::radians(60.0f), 16.0f/9.0f, 0.1f, 1000.0f);
tgl::Mat4 ortho = tgl::Mat4::ortho(0, 1280, 0, 720, -1, 1);

// Utilities
float r = tgl::math::radians(90.0f);
float v = tgl::math::lerp(0.0f, 100.0f, 0.5f);       // 50
float s = tgl::math::smoothstep(0.0f, 1.0f, 0.3f);
float c = tgl::math::clamp(x, 0.0f, 1.0f);
int rnd = tgl::math::random_int(1, 100);
```

---

## Architecture

```
include/tgl/
├── tgl.h              # Single master include
├── core/              # App, Timer, Types, Logging
├── math/              # Vec2/3/4, Mat4, Transform, math utilities
├── platform/          # Window abstraction (X11/Win32)
├── graphics/          # Renderer, Shader, Texture, Mesh, Sprite, Camera, Font
├── input/             # Keyboard, Mouse, Input manager
├── audio/             # Audio playback (WAV)
├── ecs/               # Entity Component System
└── utils/             # File I/O, Image loading (TGA/BMP)
```

## Examples

Build with `TGL_BUILD_EXAMPLES=ON` (default), then run from the `build/examples/` directory:

| Example | What it shows | Run |
|---|---|---|
| `hello_triangle` | Minimal 2D drawing with shapes | `./hello_triangle` |
| `spinning_cube` | 3D cube with camera and lighting | `./spinning_cube` |
| `sprite_demo` | 2D particle system with batching | `./sprite_demo` |
| `ecs_demo` | 500 bouncing entities using ECS | `./ecs_demo` |
| `fps_camera` | First-person camera with mouse look + WASD | `./fps_camera` |

## License

MIT
