#include <tgl/graphics/vertex.h>
#include "gl_loader.h"

namespace tgl {

// VertexBuffer
VertexBuffer::~VertexBuffer() { destroy(); }

void VertexBuffer::create(const void* data, u32 size, bool dynamic) {
    glGenBuffers(1, &id_);
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void VertexBuffer::update(const void* data, u32 size, u32 offset) {
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id_);
}

void VertexBuffer::destroy() {
    if (id_) { glDeleteBuffers(1, &id_); id_ = 0; }
}

// IndexBuffer
IndexBuffer::~IndexBuffer() { destroy(); }

void IndexBuffer::create(const u32* data, u32 count, bool dynamic) {
    count_ = count;
    glGenBuffers(1, &id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void IndexBuffer::update(const u32* data, u32 count, u32 offset) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(u32), count * sizeof(u32), data);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void IndexBuffer::destroy() {
    if (id_) { glDeleteBuffers(1, &id_); id_ = 0; count_ = 0; }
}

// VertexArray
VertexArray::~VertexArray() { destroy(); }

void VertexArray::create() {
    glGenVertexArrays(1, &id_);
}

void VertexArray::bind() const {
    glBindVertexArray(id_);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::destroy() {
    if (id_) { glDeleteVertexArrays(1, &id_); id_ = 0; }
}

void VertexArray::add_attribute(u32 index, int count, u32 stride, u64 offset, bool normalized) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, count, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, stride, (const void*)offset);
}

} // namespace tgl
