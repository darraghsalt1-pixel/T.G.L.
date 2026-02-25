#ifndef TGL_GRAPHICS_MESH_H
#define TGL_GRAPHICS_MESH_H

#include "../core/types.h"
#include "vertex.h"
#include <vector>

namespace tgl {

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    void create(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);
    void create(const Vertex* vertices, u32 vertex_count, const u32* indices, u32 index_count);
    void destroy();
    void draw() const;

    u32 vertex_count() const { return vertex_count_; }
    u32 index_count() const { return index_count_; }

    // Primitive generators
    static Mesh quad(float width = 1.0f, float height = 1.0f);
    static Mesh cube(float size = 1.0f);
    static Mesh sphere(float radius = 1.0f, int segments = 32, int rings = 16);
    static Mesh plane(float width = 10.0f, float depth = 10.0f, int subdivisions = 1);
    static Mesh cylinder(float radius = 0.5f, float height = 1.0f, int segments = 32);

private:
    VertexArray vao_;
    VertexBuffer vbo_;
    IndexBuffer ibo_;
    u32 vertex_count_ = 0;
    u32 index_count_ = 0;
};

} // namespace tgl

#endif // TGL_GRAPHICS_MESH_H
