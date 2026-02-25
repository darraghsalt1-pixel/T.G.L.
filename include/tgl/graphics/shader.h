#ifndef TGL_GRAPHICS_SHADER_H
#define TGL_GRAPHICS_SHADER_H

#include "../core/types.h"
#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat4.h"
#include "color.h"

#include <string>

namespace tgl {

class Shader {
public:
    Shader() = default;
    ~Shader();

    // Create from source strings
    Result compile(const std::string& vertex_src, const std::string& fragment_src);
    // Create from source strings with geometry shader
    Result compile(const std::string& vertex_src, const std::string& fragment_src, const std::string& geometry_src);

    void bind() const;
    void unbind() const;

    // Uniform setters
    void set_int(const char* name, int value) const;
    void set_float(const char* name, float value) const;
    void set_vec2(const char* name, const Vec2& v) const;
    void set_vec3(const char* name, const Vec3& v) const;
    void set_vec4(const char* name, const Vec4& v) const;
    void set_mat4(const char* name, const Mat4& m) const;
    void set_color(const char* name, const Color& c) const;

    u32 id() const { return id_; }
    bool valid() const { return id_ != 0; }

    // Built-in shader sources
    static const char* default_vertex_src();
    static const char* default_fragment_src();
    static const char* sprite_vertex_src();
    static const char* sprite_fragment_src();

private:
    u32 id_ = 0;
    int get_location(const char* name) const;
};

} // namespace tgl

#endif // TGL_GRAPHICS_SHADER_H
