#include <tgl/graphics/mesh.h>
#include "gl_loader.h"
#include <cmath>

namespace tgl {

Mesh::~Mesh() { destroy(); }

void Mesh::create(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    create(vertices.data(), (u32)vertices.size(), indices.data(), (u32)indices.size());
}

void Mesh::create(const Vertex* vertices, u32 vertex_count, const u32* indices, u32 index_count) {
    destroy();
    vertex_count_ = vertex_count;
    index_count_ = index_count;

    vao_.create();
    vao_.bind();

    vbo_.create(vertices, vertex_count * sizeof(Vertex));

    u32 stride = sizeof(Vertex);
    vao_.add_attribute(0, 3, stride, offsetof(Vertex, position));   // position
    vao_.add_attribute(1, 3, stride, offsetof(Vertex, normal));     // normal
    vao_.add_attribute(2, 2, stride, offsetof(Vertex, uv));         // uv
    vao_.add_attribute(3, 4, stride, offsetof(Vertex, color));      // color

    ibo_.create(indices, index_count);

    vao_.unbind();
}

void Mesh::destroy() {
    vao_.destroy();
    vbo_.destroy();
    ibo_.destroy();
    vertex_count_ = index_count_ = 0;
}

void Mesh::draw() const {
    vao_.bind();
    glDrawElements(GL_TRIANGLES, ibo_.count(), GL_UNSIGNED_INT, nullptr);
    vao_.unbind();
}

// ---- Primitive generators ----

Mesh Mesh::quad(float width, float height) {
    float hw = width * 0.5f, hh = height * 0.5f;
    std::vector<Vertex> verts = {
        {{-hw, -hh, 0}, {0, 0, 1}, {0, 0}, Color::white()},
        {{ hw, -hh, 0}, {0, 0, 1}, {1, 0}, Color::white()},
        {{ hw,  hh, 0}, {0, 0, 1}, {1, 1}, Color::white()},
        {{-hw,  hh, 0}, {0, 0, 1}, {0, 1}, Color::white()},
    };
    std::vector<u32> indices = {0, 1, 2, 2, 3, 0};
    Mesh m;
    m.create(verts, indices);
    return m;
}

Mesh Mesh::cube(float size) {
    float s = size * 0.5f;
    // 24 vertices (4 per face for proper normals)
    std::vector<Vertex> v;
    std::vector<u32> idx;

    auto face = [&](Vec3 n, Vec3 u, Vec3 r) {
        u32 base = (u32)v.size();
        v.push_back({n*s - r*s - u*s, n, {0,0}, Color::white()});
        v.push_back({n*s + r*s - u*s, n, {1,0}, Color::white()});
        v.push_back({n*s + r*s + u*s, n, {1,1}, Color::white()});
        v.push_back({n*s - r*s + u*s, n, {0,1}, Color::white()});
        idx.insert(idx.end(), {base, base+1, base+2, base+2, base+3, base});
    };

    face({ 0, 0, 1}, {0,1,0}, {1,0,0});  // front
    face({ 0, 0,-1}, {0,1,0}, {-1,0,0}); // back
    face({ 1, 0, 0}, {0,1,0}, {0,0,-1}); // right
    face({-1, 0, 0}, {0,1,0}, {0,0,1});  // left
    face({ 0, 1, 0}, {0,0,-1},{1,0,0});  // top
    face({ 0,-1, 0}, {0,0,1}, {1,0,0});  // bottom

    Mesh m;
    m.create(v, idx);
    return m;
}

Mesh Mesh::sphere(float radius, int segments, int rings) {
    std::vector<Vertex> verts;
    std::vector<u32> indices;

    for (int y = 0; y <= rings; y++) {
        for (int x = 0; x <= segments; x++) {
            float xf = (float)x / segments;
            float yf = (float)y / rings;
            float theta = xf * 2.0f * 3.14159265f;
            float phi   = yf * 3.14159265f;

            Vec3 pos = {
                std::cos(theta) * std::sin(phi) * radius,
                std::cos(phi) * radius,
                std::sin(theta) * std::sin(phi) * radius
            };
            Vec3 norm = pos.normalized();
            verts.push_back({pos, norm, {xf, yf}, Color::white()});
        }
    }

    for (int y = 0; y < rings; y++) {
        for (int x = 0; x < segments; x++) {
            u32 a = y * (segments + 1) + x;
            u32 b = a + segments + 1;
            indices.insert(indices.end(), {a, b, a + 1, b, b + 1, a + 1});
        }
    }

    Mesh m;
    m.create(verts, indices);
    return m;
}

Mesh Mesh::plane(float width, float depth, int subdivisions) {
    std::vector<Vertex> verts;
    std::vector<u32> indices;

    int verts_per_side = subdivisions + 1;
    for (int z = 0; z < verts_per_side; z++) {
        for (int x = 0; x < verts_per_side; x++) {
            float fx = (float)x / subdivisions;
            float fz = (float)z / subdivisions;
            verts.push_back({
                {(fx - 0.5f) * width, 0, (fz - 0.5f) * depth},
                {0, 1, 0},
                {fx, fz},
                Color::white()
            });
        }
    }

    for (int z = 0; z < subdivisions; z++) {
        for (int x = 0; x < subdivisions; x++) {
            u32 a = z * verts_per_side + x;
            u32 b = a + verts_per_side;
            indices.insert(indices.end(), {a, b, a + 1, b, b + 1, a + 1});
        }
    }

    Mesh m;
    m.create(verts, indices);
    return m;
}

Mesh Mesh::cylinder(float radius, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<u32> indices;
    float hh = height * 0.5f;

    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * 3.14159265f;
        float cx = std::cos(angle), cz = std::sin(angle);
        Vec3 norm = {cx, 0, cz};
        float u = (float)i / segments;
        verts.push_back({{cx * radius, -hh, cz * radius}, norm, {u, 0}, Color::white()});
        verts.push_back({{cx * radius,  hh, cz * radius}, norm, {u, 1}, Color::white()});
    }
    for (int i = 0; i < segments; i++) {
        u32 a = i * 2, b = a + 1, c = a + 2, d = a + 3;
        indices.insert(indices.end(), {a, c, b, b, c, d});
    }

    // Top and bottom caps
    u32 top_center = (u32)verts.size();
    verts.push_back({{0, hh, 0}, {0, 1, 0}, {0.5f, 0.5f}, Color::white()});
    u32 bot_center = (u32)verts.size();
    verts.push_back({{0, -hh, 0}, {0, -1, 0}, {0.5f, 0.5f}, Color::white()});

    for (int i = 0; i < segments; i++) {
        float a0 = (float)i / segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;
        u32 base = (u32)verts.size();
        // Top cap
        verts.push_back({{std::cos(a0)*radius, hh, std::sin(a0)*radius}, {0,1,0}, {0,0}, Color::white()});
        verts.push_back({{std::cos(a1)*radius, hh, std::sin(a1)*radius}, {0,1,0}, {0,0}, Color::white()});
        indices.insert(indices.end(), {top_center, base, base + 1});
        // Bottom cap
        base = (u32)verts.size();
        verts.push_back({{std::cos(a1)*radius, -hh, std::sin(a1)*radius}, {0,-1,0}, {0,0}, Color::white()});
        verts.push_back({{std::cos(a0)*radius, -hh, std::sin(a0)*radius}, {0,-1,0}, {0,0}, Color::white()});
        indices.insert(indices.end(), {bot_center, base, base + 1});
    }

    Mesh m;
    m.create(verts, indices);
    return m;
}

} // namespace tgl
