# T.G.L. — The Graphics Library

A lightweight, easy-to-use C++17 graphics library built for games and simulations.

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
- **Audio** — WAV loading and playback (basic)
- **Font rendering** — built-in bitmap font, TTF via stb_truetype integration

## Quick Start

```cpp
#include <tgl/tgl.h>

int main() {
    tgl::App app;

    app.on_update = [](float dt) {
        if (tgl::App::instance().input().key_pressed(tgl::Key::Escape))
            tgl::App::instance().quit();
    };

    app.on_render = []() {
        auto& r = tgl::App::instance().renderer();
        r.begin_2d(1280, 720);
        r.draw_rect({100, 100}, {200, 150}, tgl::Color::cyan());
        r.draw_circle({640, 360}, 80, tgl::Color::red());
        r.end_2d();
    };

    app.init({"My Game", 1280, 720});
    app.run();
}
```

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Options

| CMake Option | Default | Description |
|---|---|---|
| `TGL_BUILD_EXAMPLES` | ON | Build example programs |
| `TGL_BUILD_TESTS` | OFF | Build tests |
| `TGL_USE_WAYLAND` | OFF | Use Wayland instead of X11 on Linux |

### Dependencies

- OpenGL 3.3+ drivers
- Linux: `libx11-dev` (or Wayland dev packages)
- Windows: no extra dependencies
- macOS: Cocoa framework (included)

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

| Example | Description |
|---|---|
| `hello_triangle` | Minimal 2D drawing with shapes |
| `spinning_cube` | 3D cube with camera and lighting |
| `sprite_demo` | 2D particle system with batching |
| `ecs_demo` | 500 entities with ECS |
| `fps_camera` | First-person camera with mouse look |

## License

MIT
