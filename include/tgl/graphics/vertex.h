#ifndef TGL_GRAPHICS_VERTEX_H
#define TGL_GRAPHICS_VERTEX_H

#include "../math/vec2.h"
#include "../math/vec3.h"
#include "color.h"

namespace tgl {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    Color color = Color::white();
};

struct Vertex2D {
    Vec2 position;
    Vec2 uv;
    Color color = Color::white();
};

// GPU buffer handle
class VertexBuffer {
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    void create(const void* data, u32 size, bool dynamic = false);
    void update(const void* data, u32 size, u32 offset = 0);
    void bind() const;
    void destroy();

    u32 id() const { return id_; }

private:
    u32 id_ = 0;
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    ~IndexBuffer();

    void create(const u32* data, u32 count, bool dynamic = false);
    void update(const u32* data, u32 count, u32 offset = 0);
    void bind() const;
    void destroy();

    u32 count() const { return count_; }
    u32 id() const { return id_; }

private:
    u32 id_ = 0;
    u32 count_ = 0;
};

class VertexArray {
public:
    VertexArray() = default;
    ~VertexArray();

    void create();
    void bind() const;
    void unbind() const;
    void destroy();

    // Add attribute: index, component count, stride, offset
    void add_attribute(u32 index, int count, u32 stride, u64 offset, bool normalized = false);

    u32 id() const { return id_; }

private:
    u32 id_ = 0;
};

} // namespace tgl

#endif // TGL_GRAPHICS_VERTEX_H
