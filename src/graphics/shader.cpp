#include <tgl/graphics/shader.h>
#include <tgl/core/log.h>
#include "gl_loader.h"

namespace tgl {

Shader::~Shader() {
    if (id_) glDeleteProgram(id_);
}

static u32 compile_shader(GLenum type, const std::string& source) {
    u32 shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        const char* type_str = (type == GL_VERTEX_SHADER) ? "Vertex" :
                               (type == GL_FRAGMENT_SHADER) ? "Fragment" : "Geometry";
        log::error("%s shader compilation failed: %s", type_str, info);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Result Shader::compile(const std::string& vertex_src, const std::string& fragment_src) {
    u32 vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    if (!vs) return {ErrorCode::ShaderCompileFailed, "Vertex shader compilation failed"};

    u32 fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (!fs) { glDeleteShader(vs); return {ErrorCode::ShaderCompileFailed, "Fragment shader compilation failed"}; }

    id_ = glCreateProgram();
    glAttachShader(id_, vs);
    glAttachShader(id_, fs);
    glLinkProgram(id_);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(id_, 512, nullptr, info);
        log::error("Shader link failed: %s", info);
        glDeleteProgram(id_);
        id_ = 0;
        return {ErrorCode::ShaderLinkFailed, "Shader link failed"};
    }

    return {};
}

Result Shader::compile(const std::string& vertex_src, const std::string& fragment_src, const std::string& geometry_src) {
    u32 vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    if (!vs) return {ErrorCode::ShaderCompileFailed, "Vertex shader compilation failed"};

    u32 fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (!fs) { glDeleteShader(vs); return {ErrorCode::ShaderCompileFailed, "Fragment shader compilation failed"}; }

    u32 gs = compile_shader(GL_GEOMETRY_SHADER, geometry_src);
    if (!gs) { glDeleteShader(vs); glDeleteShader(fs); return {ErrorCode::ShaderCompileFailed, "Geometry shader compilation failed"}; }

    id_ = glCreateProgram();
    glAttachShader(id_, vs);
    glAttachShader(id_, fs);
    glAttachShader(id_, gs);
    glLinkProgram(id_);

    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);

    GLint success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(id_, 512, nullptr, info);
        log::error("Shader link failed: %s", info);
        glDeleteProgram(id_);
        id_ = 0;
        return {ErrorCode::ShaderLinkFailed, "Shader link failed"};
    }

    return {};
}

void Shader::bind() const { glUseProgram(id_); }
void Shader::unbind() const { glUseProgram(0); }

int Shader::get_location(const char* name) const {
    return glGetUniformLocation(id_, name);
}

void Shader::set_int(const char* name, int value) const {
    glUniform1i(get_location(name), value);
}

void Shader::set_float(const char* name, float value) const {
    glUniform1f(get_location(name), value);
}

void Shader::set_vec2(const char* name, const Vec2& v) const {
    glUniform2f(get_location(name), v.x, v.y);
}

void Shader::set_vec3(const char* name, const Vec3& v) const {
    glUniform3f(get_location(name), v.x, v.y, v.z);
}

void Shader::set_vec4(const char* name, const Vec4& v) const {
    glUniform4f(get_location(name), v.x, v.y, v.z, v.w);
}

void Shader::set_mat4(const char* name, const Mat4& m) const {
    glUniformMatrix4fv(get_location(name), 1, GL_FALSE, m.data());
}

void Shader::set_color(const char* name, const Color& c) const {
    glUniform4f(get_location(name), c.r, c.g, c.b, c.a);
}

// Default 3D shader
const char* Shader::default_vertex_src() {
    return R"(
#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec4 a_color;

uniform mat4 u_model;
uniform mat4 u_view_projection;

out vec3 v_normal;
out vec2 v_uv;
out vec4 v_color;
out vec3 v_world_pos;

void main() {
    vec4 world = u_model * vec4(a_position, 1.0);
    v_world_pos = world.xyz;
    v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_uv = a_uv;
    v_color = a_color;
    gl_Position = u_view_projection * world;
}
)";
}

const char* Shader::default_fragment_src() {
    return R"(
#version 330 core
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;
in vec3 v_world_pos;

uniform sampler2D u_texture;
uniform vec4 u_tint = vec4(1.0);
uniform vec3 u_light_dir = normalize(vec3(0.5, 1.0, 0.3));
uniform vec3 u_light_color = vec3(1.0);
uniform vec3 u_ambient = vec3(0.15);

out vec4 frag_color;

void main() {
    vec3 norm = normalize(v_normal);
    float diff = max(dot(norm, u_light_dir), 0.0);
    vec3 lighting = u_ambient + u_light_color * diff;

    vec4 tex = texture(u_texture, v_uv);
    frag_color = tex * v_color * u_tint * vec4(lighting, 1.0);
}
)";
}

// 2D sprite shader
const char* Shader::sprite_vertex_src() {
    return R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;

uniform mat4 u_view_projection;

out vec2 v_uv;
out vec4 v_color;

void main() {
    v_uv = a_uv;
    v_color = a_color;
    gl_Position = u_view_projection * vec4(a_position, 0.0, 1.0);
}
)";
}

const char* Shader::sprite_fragment_src() {
    return R"(
#version 330 core
in vec2 v_uv;
in vec4 v_color;

uniform sampler2D u_texture;

out vec4 frag_color;

void main() {
    frag_color = texture(u_texture, v_uv) * v_color;
    if (frag_color.a < 0.01) discard;
}
)";
}

} // namespace tgl
