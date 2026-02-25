#ifndef TGL_GRAPHICS_RENDERER_H
#define TGL_GRAPHICS_RENDERER_H

#include "../core/types.h"
#include "color.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "sprite.h"
#include "font.h"
#include "camera.h"
#include "../math/mat4.h"

#include <vector>
#include <string>

namespace tgl {

// Render states
enum class BlendMode { None, Alpha, Additive, Multiply };
enum class CullFace  { None, Back, Front };
enum class DepthFunc { Always, Less, LessEqual, Greater, Equal };

struct RenderState {
    BlendMode blend = BlendMode::Alpha;
    CullFace cull = CullFace::Back;
    DepthFunc depth = DepthFunc::Less;
    bool depth_write = true;
    bool wireframe = false;
};

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    Result init();
    void shutdown();

    // Frame management
    void begin_frame(const Color& clear_color = Color::cornflower());
    void end_frame();

    // State
    void set_viewport(int x, int y, int width, int height);
    void set_render_state(const RenderState& state);
    void set_blend_mode(BlendMode mode);
    void set_depth_test(bool enabled);

    // 3D rendering
    void begin_3d(const Camera& camera, float aspect);
    void draw_mesh(const Mesh& mesh, const Shader& shader, const Mat4& transform = Mat4());
    void end_3d();

    // 2D batch rendering (auto-batched for performance)
    void begin_2d(const Camera2D& camera, float screen_width, float screen_height);
    void begin_2d(float screen_width, float screen_height); // default camera
    void draw_sprite(const Sprite& sprite);
    void draw_rect(const Vec2& pos, const Vec2& size, const Color& color);
    void draw_rect_outline(const Vec2& pos, const Vec2& size, const Color& color, float thickness = 1.0f);
    void draw_circle(const Vec2& center, float radius, const Color& color, int segments = 32);
    void draw_line(const Vec2& start, const Vec2& end, const Color& color, float thickness = 1.0f);
    void draw_text(const Font& font, const std::string& text, const Vec2& pos, const Color& color = Color::white(), float scale = 1.0f);
    void end_2d();

    // Immediate-mode debug drawing (3D)
    void debug_line(const Vec3& start, const Vec3& end, const Color& color = Color::green());
    void debug_box(const Vec3& center, const Vec3& size, const Color& color = Color::green());
    void debug_sphere(const Vec3& center, float radius, const Color& color = Color::green(), int segments = 16);

    // Stats
    int draw_calls() const { return draw_calls_; }
    int triangles() const { return triangles_; }

    const Shader& default_shader() const { return default_shader_; }
    const Shader& sprite_shader() const { return sprite_shader_; }

private:
    // Internal batch data
    struct SpriteBatch;

    Shader default_shader_;
    Shader sprite_shader_;
    Shader debug_shader_;

    Mat4 view_projection_;

    // Sprite batching
    std::vector<Vertex2D> batch_vertices_;
    std::vector<u32> batch_indices_;
    VertexArray batch_vao_;
    VertexBuffer batch_vbo_;
    IndexBuffer batch_ibo_;
    Texture* current_batch_texture_ = nullptr;
    u32 batch_count_ = 0;
    static constexpr u32 MAX_BATCH_SPRITES = 10000;

    // Debug lines
    struct DebugLine { Vec3 start, end; Color color; };
    std::vector<DebugLine> debug_lines_;
    VertexArray debug_vao_;
    VertexBuffer debug_vbo_;

    int draw_calls_ = 0;
    int triangles_ = 0;

    void flush_batch();
    void init_batch_buffers();
    void init_debug_buffers();
};

// Framebuffer / Render target
class RenderTarget {
public:
    RenderTarget() = default;
    ~RenderTarget();

    Result create(int width, int height, bool with_depth = true);
    void destroy();

    void bind();
    void unbind();

    const Texture& color_texture() const { return color_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    u32 fbo_ = 0;
    u32 depth_rbo_ = 0;
    Texture color_;
    int width_ = 0;
    int height_ = 0;
};

} // namespace tgl

#endif // TGL_GRAPHICS_RENDERER_H
