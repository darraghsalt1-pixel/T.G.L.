#include <tgl/graphics/renderer.h>
#include <tgl/core/log.h>
#include "gl_loader.h"
#include <algorithm>
#include <cstring>

namespace tgl {

Renderer::~Renderer() { shutdown(); }

Result Renderer::init() {
    // Compile built-in shaders
    auto r = default_shader_.compile(Shader::default_vertex_src(), Shader::default_fragment_src());
    if (!r) return r;

    r = sprite_shader_.compile(Shader::sprite_vertex_src(), Shader::sprite_fragment_src());
    if (!r) return r;

    // Debug line shader (simple passthrough)
    const char* debug_vs = R"(
#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
uniform mat4 u_view_projection;
out vec4 v_color;
void main() {
    v_color = a_color;
    gl_Position = u_view_projection * vec4(a_position, 1.0);
}
)";
    const char* debug_fs = R"(
#version 330 core
in vec4 v_color;
out vec4 frag_color;
void main() { frag_color = v_color; }
)";
    r = debug_shader_.compile(debug_vs, debug_fs);
    if (!r) return r;

    init_batch_buffers();
    init_debug_buffers();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    log::info("Renderer initialized (OpenGL %s)", (const char*)glGetString(GL_VERSION));
    return {};
}

void Renderer::shutdown() {
    batch_vao_.destroy();
    batch_vbo_.destroy();
    batch_ibo_.destroy();
    debug_vao_.destroy();
    debug_vbo_.destroy();
}

void Renderer::begin_frame(const Color& clear_color) {
    draw_calls_ = 0;
    triangles_ = 0;
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end_frame() {
    // Flush any remaining debug lines
    if (!debug_lines_.empty()) {
        debug_shader_.bind();
        debug_shader_.set_mat4("u_view_projection", view_projection_);

        struct DebugVert { Vec3 pos; Color col; };
        std::vector<DebugVert> verts;
        verts.reserve(debug_lines_.size() * 2);
        for (auto& l : debug_lines_) {
            verts.push_back({l.start, l.color});
            verts.push_back({l.end, l.color});
        }

        debug_vao_.bind();
        debug_vbo_.update(verts.data(), (u32)(verts.size() * sizeof(DebugVert)));
        glDrawArrays(GL_LINES, 0, (GLsizei)verts.size());
        draw_calls_++;
        debug_vao_.unbind();
        debug_lines_.clear();
    }
}

void Renderer::set_viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::set_blend_mode(BlendMode mode) {
    switch (mode) {
        case BlendMode::None:
            glDisable(GL_BLEND);
            break;
        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BlendMode::Multiply:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
    }
}

void Renderer::set_depth_test(bool enabled) {
    if (enabled) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}

void Renderer::set_render_state(const RenderState& state) {
    set_blend_mode(state.blend);
    set_depth_test(state.depth != DepthFunc::Always);
    glDepthMask(state.depth_write ? GL_TRUE : GL_FALSE);
    glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);

    switch (state.cull) {
        case CullFace::None:  glDisable(GL_CULL_FACE); break;
        case CullFace::Back:  glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
        case CullFace::Front: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
    }
}

// ---- 3D ----

void Renderer::begin_3d(const Camera& camera, float aspect) {
    view_projection_ = camera.view_projection(aspect);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void Renderer::draw_mesh(const Mesh& mesh, const Shader& shader, const Mat4& transform) {
    shader.bind();
    shader.set_mat4("u_model", transform);
    shader.set_mat4("u_view_projection", view_projection_);
    Texture::white_texture().bind(0);
    shader.set_int("u_texture", 0);
    mesh.draw();
    draw_calls_++;
    triangles_ += mesh.index_count() / 3;
}

void Renderer::end_3d() {}

// ---- 2D Batching ----

void Renderer::init_batch_buffers() {
    batch_vertices_.reserve(MAX_BATCH_SPRITES * 4);
    batch_indices_.reserve(MAX_BATCH_SPRITES * 6);

    batch_vao_.create();
    batch_vao_.bind();

    batch_vbo_.create(nullptr, MAX_BATCH_SPRITES * 4 * sizeof(Vertex2D), true);

    u32 stride = sizeof(Vertex2D);
    batch_vao_.add_attribute(0, 2, stride, offsetof(Vertex2D, position));
    batch_vao_.add_attribute(1, 2, stride, offsetof(Vertex2D, uv));
    batch_vao_.add_attribute(2, 4, stride, offsetof(Vertex2D, color));

    // Pre-generate indices
    std::vector<u32> indices(MAX_BATCH_SPRITES * 6);
    for (u32 i = 0; i < MAX_BATCH_SPRITES; i++) {
        u32 base = i * 4;
        indices[i * 6 + 0] = base;
        indices[i * 6 + 1] = base + 1;
        indices[i * 6 + 2] = base + 2;
        indices[i * 6 + 3] = base + 2;
        indices[i * 6 + 4] = base + 3;
        indices[i * 6 + 5] = base;
    }
    batch_ibo_.create(indices.data(), (u32)indices.size());

    batch_vao_.unbind();
}

void Renderer::init_debug_buffers() {
    debug_vao_.create();
    debug_vao_.bind();
    debug_vbo_.create(nullptr, 65536, true); // 64KB for debug lines

    // position (vec3) + color (vec4) = 7 floats = 28 bytes
    u32 stride = sizeof(Vec3) + sizeof(Color);
    debug_vao_.add_attribute(0, 3, stride, 0);
    debug_vao_.add_attribute(1, 4, stride, sizeof(Vec3));
    debug_vao_.unbind();
}

void Renderer::begin_2d(const Camera2D& camera, float screen_width, float screen_height) {
    view_projection_ = camera.view_projection(screen_width, screen_height);
    glDisable(GL_DEPTH_TEST);
    set_blend_mode(BlendMode::Alpha);
    sprite_shader_.bind();
    sprite_shader_.set_mat4("u_view_projection", view_projection_);
    sprite_shader_.set_int("u_texture", 0);

    batch_vertices_.clear();
    batch_indices_.clear();
    batch_count_ = 0;
    current_batch_texture_ = nullptr;
}

void Renderer::begin_2d(float screen_width, float screen_height) {
    Camera2D cam;
    begin_2d(cam, screen_width, screen_height);
}

void Renderer::flush_batch() {
    if (batch_count_ == 0) return;

    if (current_batch_texture_) current_batch_texture_->bind(0);
    else Texture::white_texture().bind(0);

    batch_vao_.bind();
    batch_vbo_.update(batch_vertices_.data(), (u32)(batch_vertices_.size() * sizeof(Vertex2D)));
    glDrawElements(GL_TRIANGLES, batch_count_ * 6, GL_UNSIGNED_INT, nullptr);
    draw_calls_++;
    triangles_ += batch_count_ * 2;

    batch_vao_.unbind();
    batch_vertices_.clear();
    batch_count_ = 0;
}

void Renderer::draw_sprite(const Sprite& sprite) {
    Texture* tex = sprite.texture ? sprite.texture : &Texture::white_texture();
    if (current_batch_texture_ != tex) {
        flush_batch();
        current_batch_texture_ = tex;
    }
    if (batch_count_ >= MAX_BATCH_SPRITES) flush_batch();

    float ox = sprite.origin.x * sprite.size.x;
    float oy = sprite.origin.y * sprite.size.y;

    Vec2 corners[4] = {
        {-ox, -oy},
        {sprite.size.x - ox, -oy},
        {sprite.size.x - ox, sprite.size.y - oy},
        {-ox, sprite.size.y - oy}
    };

    // Rotate
    if (sprite.rotation != 0.0f) {
        for (auto& c : corners) c = c.rotated(sprite.rotation);
    }

    // UV
    float u0 = sprite.uv_rect.x, v0 = sprite.uv_rect.y;
    float u1 = u0 + sprite.uv_rect.w, v1 = v0 + sprite.uv_rect.h;
    if (sprite.flip_x) std::swap(u0, u1);
    if (sprite.flip_y) std::swap(v0, v1);

    Vec2 uvs[4] = {{u0,v0}, {u1,v0}, {u1,v1}, {u0,v1}};

    for (int i = 0; i < 4; i++) {
        batch_vertices_.push_back({
            sprite.position + corners[i],
            uvs[i],
            sprite.tint
        });
    }

    batch_count_++;
}

void Renderer::draw_rect(const Vec2& pos, const Vec2& size, const Color& color) {
    Sprite s;
    s.position = pos;
    s.size = size;
    s.origin = {0, 0};
    s.tint = color;
    draw_sprite(s);
}

void Renderer::draw_rect_outline(const Vec2& pos, const Vec2& size, const Color& color, float thickness) {
    draw_rect(pos, {size.x, thickness}, color);                             // top
    draw_rect({pos.x, pos.y + size.y - thickness}, {size.x, thickness}, color); // bottom
    draw_rect(pos, {thickness, size.y}, color);                             // left
    draw_rect({pos.x + size.x - thickness, pos.y}, {thickness, size.y}, color); // right
}

void Renderer::draw_circle(const Vec2& center, float radius, const Color& color, int segments) {
    // Approximate circle with triangle fan using sprites
    for (int i = 0; i < segments; i++) {
        float a0 = (float)i / segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;

        if (current_batch_texture_ != &Texture::white_texture()) {
            flush_batch();
            current_batch_texture_ = &Texture::white_texture();
        }
        if (batch_count_ >= MAX_BATCH_SPRITES) flush_batch();

        Vec2 p0 = center;
        Vec2 p1 = center + Vec2{std::cos(a0) * radius, std::sin(a0) * radius};
        Vec2 p2 = center + Vec2{std::cos(a1) * radius, std::sin(a1) * radius};

        batch_vertices_.push_back({p0, {0,0}, color});
        batch_vertices_.push_back({p1, {0,0}, color});
        batch_vertices_.push_back({p2, {0,0}, color});
        batch_vertices_.push_back({p2, {0,0}, color}); // degenerate quad
        batch_count_++;
    }
}

void Renderer::draw_line(const Vec2& start, const Vec2& end, const Color& color, float thickness) {
    Vec2 dir = (end - start).normalized();
    Vec2 perp = dir.perpendicular() * (thickness * 0.5f);

    if (current_batch_texture_ != &Texture::white_texture()) {
        flush_batch();
        current_batch_texture_ = &Texture::white_texture();
    }
    if (batch_count_ >= MAX_BATCH_SPRITES) flush_batch();

    batch_vertices_.push_back({start + perp, {0,0}, color});
    batch_vertices_.push_back({start - perp, {0,0}, color});
    batch_vertices_.push_back({end - perp, {0,0}, color});
    batch_vertices_.push_back({end + perp, {0,0}, color});
    batch_count_++;
}

void Renderer::draw_text(const Font& font, const std::string& text, const Vec2& pos, const Color& color, float scale) {
    if (!font.valid()) return;

    // Switch to font atlas texture
    Texture* atlas = const_cast<Texture*>(&font.atlas());
    if (current_batch_texture_ != atlas) {
        flush_batch();
        current_batch_texture_ = atlas;
    }

    float x = pos.x, y = pos.y;
    for (char c : text) {
        if (c == '\n') {
            x = pos.x;
            y += font.line_height() * scale;
            continue;
        }

        const GlyphInfo* g = font.glyph((u32)c);
        if (!g) { x += font.size() * 0.5f * scale; continue; }

        if (batch_count_ >= MAX_BATCH_SPRITES) flush_batch();

        float gx = x + g->bearing.x * scale;
        float gy = y + (font.line_height() - g->bearing.y) * scale;
        float gw = g->size.x * scale;
        float gh = g->size.y * scale;

        batch_vertices_.push_back({{gx, gy},         g->uv_min, color});
        batch_vertices_.push_back({{gx + gw, gy},    {g->uv_max.x, g->uv_min.y}, color});
        batch_vertices_.push_back({{gx + gw, gy + gh}, g->uv_max, color});
        batch_vertices_.push_back({{gx, gy + gh},    {g->uv_min.x, g->uv_max.y}, color});
        batch_count_++;

        x += g->advance * scale;
    }
}

void Renderer::end_2d() {
    flush_batch();
    glEnable(GL_DEPTH_TEST);
}

// Debug drawing
void Renderer::debug_line(const Vec3& start, const Vec3& end, const Color& color) {
    debug_lines_.push_back({start, end, color});
}

void Renderer::debug_box(const Vec3& center, const Vec3& size, const Color& color) {
    Vec3 h = size * 0.5f;
    Vec3 corners[8] = {
        center + Vec3{-h.x, -h.y, -h.z}, center + Vec3{ h.x, -h.y, -h.z},
        center + Vec3{ h.x,  h.y, -h.z}, center + Vec3{-h.x,  h.y, -h.z},
        center + Vec3{-h.x, -h.y,  h.z}, center + Vec3{ h.x, -h.y,  h.z},
        center + Vec3{ h.x,  h.y,  h.z}, center + Vec3{-h.x,  h.y,  h.z},
    };
    // 12 edges
    int edges[][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for (auto& e : edges) debug_line(corners[e[0]], corners[e[1]], color);
}

void Renderer::debug_sphere(const Vec3& center, float radius, const Color& color, int segments) {
    for (int i = 0; i < segments; i++) {
        float a0 = (float)i / segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;
        // XY circle
        debug_line(center + Vec3{std::cos(a0)*radius, std::sin(a0)*radius, 0},
                   center + Vec3{std::cos(a1)*radius, std::sin(a1)*radius, 0}, color);
        // XZ circle
        debug_line(center + Vec3{std::cos(a0)*radius, 0, std::sin(a0)*radius},
                   center + Vec3{std::cos(a1)*radius, 0, std::sin(a1)*radius}, color);
        // YZ circle
        debug_line(center + Vec3{0, std::cos(a0)*radius, std::sin(a0)*radius},
                   center + Vec3{0, std::cos(a1)*radius, std::sin(a1)*radius}, color);
    }
}

// ---- RenderTarget ----

RenderTarget::~RenderTarget() { destroy(); }

Result RenderTarget::create(int width, int height, bool with_depth) {
    destroy();
    width_ = width;
    height_ = height;

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    TextureConfig cfg;
    cfg.generate_mipmaps = false;
    cfg.min_filter = TextureFilter::Linear;
    cfg.mag_filter = TextureFilter::Linear;
    cfg.wrap_s = TextureWrap::Clamp;
    cfg.wrap_t = TextureWrap::Clamp;
    color_.create_empty(width, height, TextureFormat::RGBA, cfg);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_.id(), 0);

    if (with_depth) {
        glGenRenderbuffers(1, &depth_rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        destroy();
        return {ErrorCode::OpenGLInitFailed, "Framebuffer incomplete"};
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return {};
}

void RenderTarget::destroy() {
    if (depth_rbo_) { glDeleteRenderbuffers(1, &depth_rbo_); depth_rbo_ = 0; }
    if (fbo_) { glDeleteFramebuffers(1, &fbo_); fbo_ = 0; }
    color_.destroy();
}

void RenderTarget::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, width_, height_);
}

void RenderTarget::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace tgl
