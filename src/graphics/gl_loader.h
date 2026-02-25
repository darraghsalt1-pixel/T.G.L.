/*
 * Minimal OpenGL 3.3+ function loader for T.G.L.
 * No external dependencies - loads GL functions at runtime.
 */
#ifndef TGL_GL_LOADER_H
#define TGL_GL_LOADER_H

#ifdef TGL_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <GL/gl.h>
#elif defined(TGL_PLATFORM_MACOS)
    #include <OpenGL/gl3.h>
#else
    #include <GL/gl.h>
#endif

#include <cstdint>
#include <cstddef>

// GL type aliases if not provided
#ifndef GL_VERSION_2_0
typedef char GLchar;
#endif

// OpenGL 3.3 constants we need
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_MIRRORED_REPEAT                0x8370
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_MULTISAMPLE                    0x809D
#define GL_TEXTURE_MAX_ANISOTROPY         0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY     0x84FF
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_RG                             0x8227
#define GL_RED                            0x1903
#endif

#ifndef GL_VERSION_1_5
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
#endif

// GL function pointer types and declarations
#define TGL_GL_FUNCTIONS \
    X(void, glGenVertexArrays, (GLsizei n, GLuint* arrays)) \
    X(void, glDeleteVertexArrays, (GLsizei n, const GLuint* arrays)) \
    X(void, glBindVertexArray, (GLuint array)) \
    X(void, glGenBuffers, (GLsizei n, GLuint* buffers)) \
    X(void, glDeleteBuffers, (GLsizei n, const GLuint* buffers)) \
    X(void, glBindBuffer, (GLenum target, GLuint buffer)) \
    X(void, glBufferData, (GLenum target, GLsizeiptr size, const void* data, GLenum usage)) \
    X(void, glBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const void* data)) \
    X(void, glEnableVertexAttribArray, (GLuint index)) \
    X(void, glDisableVertexAttribArray, (GLuint index)) \
    X(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)) \
    X(GLuint, glCreateShader, (GLenum type)) \
    X(void, glDeleteShader, (GLuint shader)) \
    X(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar** string, const GLint* length)) \
    X(void, glCompileShader, (GLuint shader)) \
    X(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params)) \
    X(void, glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)) \
    X(GLuint, glCreateProgram, (void)) \
    X(void, glDeleteProgram, (GLuint program)) \
    X(void, glAttachShader, (GLuint program, GLuint shader)) \
    X(void, glLinkProgram, (GLuint program)) \
    X(void, glGetProgramiv, (GLuint program, GLenum pname, GLint* params)) \
    X(void, glGetProgramInfoLog, (GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)) \
    X(void, glUseProgram, (GLuint program)) \
    X(GLint, glGetUniformLocation, (GLuint program, const GLchar* name)) \
    X(void, glUniform1i, (GLint location, GLint v0)) \
    X(void, glUniform1f, (GLint location, GLfloat v0)) \
    X(void, glUniform2f, (GLint location, GLfloat v0, GLfloat v1)) \
    X(void, glUniform3f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)) \
    X(void, glUniform4f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)) \
    X(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)) \
    X(void, glActiveTexture, (GLenum texture)) \
    X(void, glGenerateMipmap, (GLenum target)) \
    X(void, glGenFramebuffers, (GLsizei n, GLuint* framebuffers)) \
    X(void, glDeleteFramebuffers, (GLsizei n, const GLuint* framebuffers)) \
    X(void, glBindFramebuffer, (GLenum target, GLuint framebuffer)) \
    X(void, glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)) \
    X(GLenum, glCheckFramebufferStatus, (GLenum target)) \
    X(void, glGenRenderbuffers, (GLsizei n, GLuint* renderbuffers)) \
    X(void, glDeleteRenderbuffers, (GLsizei n, const GLuint* renderbuffers)) \
    X(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer)) \
    X(void, glRenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)) \
    X(void, glFramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)) \
    X(void, glBlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)) \
    X(void, glBlendEquation, (GLenum mode)) \
    X(void, glDrawElementsBaseVertex, (GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex))

// Declare function pointers as extern
#define X(ret, name, args) typedef ret (*PFN_##name) args; extern PFN_##name tgl_##name;
TGL_GL_FUNCTIONS
#undef X

// Remap names so code can use glXxx directly
#define glGenVertexArrays       tgl_glGenVertexArrays
#define glDeleteVertexArrays    tgl_glDeleteVertexArrays
#define glBindVertexArray       tgl_glBindVertexArray
#define glGenBuffers            tgl_glGenBuffers
#define glDeleteBuffers         tgl_glDeleteBuffers
#define glBindBuffer            tgl_glBindBuffer
#define glBufferData            tgl_glBufferData
#define glBufferSubData         tgl_glBufferSubData
#define glEnableVertexAttribArray  tgl_glEnableVertexAttribArray
#define glDisableVertexAttribArray tgl_glDisableVertexAttribArray
#define glVertexAttribPointer   tgl_glVertexAttribPointer
#define glCreateShader          tgl_glCreateShader
#define glDeleteShader          tgl_glDeleteShader
#define glShaderSource          tgl_glShaderSource
#define glCompileShader         tgl_glCompileShader
#define glGetShaderiv           tgl_glGetShaderiv
#define glGetShaderInfoLog      tgl_glGetShaderInfoLog
#define glCreateProgram         tgl_glCreateProgram
#define glDeleteProgram         tgl_glDeleteProgram
#define glAttachShader          tgl_glAttachShader
#define glLinkProgram           tgl_glLinkProgram
#define glGetProgramiv          tgl_glGetProgramiv
#define glGetProgramInfoLog     tgl_glGetProgramInfoLog
#define glUseProgram            tgl_glUseProgram
#define glGetUniformLocation    tgl_glGetUniformLocation
#define glUniform1i             tgl_glUniform1i
#define glUniform1f             tgl_glUniform1f
#define glUniform2f             tgl_glUniform2f
#define glUniform3f             tgl_glUniform3f
#define glUniform4f             tgl_glUniform4f
#define glUniformMatrix4fv      tgl_glUniformMatrix4fv
#define glActiveTexture         tgl_glActiveTexture
#define glGenerateMipmap        tgl_glGenerateMipmap
#define glGenFramebuffers       tgl_glGenFramebuffers
#define glDeleteFramebuffers    tgl_glDeleteFramebuffers
#define glBindFramebuffer       tgl_glBindFramebuffer
#define glFramebufferTexture2D  tgl_glFramebufferTexture2D
#define glCheckFramebufferStatus tgl_glCheckFramebufferStatus
#define glGenRenderbuffers      tgl_glGenRenderbuffers
#define glDeleteRenderbuffers   tgl_glDeleteRenderbuffers
#define glBindRenderbuffer      tgl_glBindRenderbuffer
#define glRenderbufferStorage   tgl_glRenderbufferStorage
#define glFramebufferRenderbuffer tgl_glFramebufferRenderbuffer
#define glBlendFuncSeparate     tgl_glBlendFuncSeparate
#define glBlendEquation         tgl_glBlendEquation
#define glDrawElementsBaseVertex tgl_glDrawElementsBaseVertex

namespace tgl {
    bool load_gl_functions();
}

#endif // TGL_GL_LOADER_H
