#ifndef TGL_GRAPHICS_CAMERA_H
#define TGL_GRAPHICS_CAMERA_H

#include "../math/vec3.h"
#include "../math/mat4.h"
#include "../math/math_utils.h"

namespace tgl {

class Camera {
public:
    Vec3 position = {0, 0, 3};
    float yaw   = -90.0f; // degrees
    float pitch  = 0.0f;
    float fov    = 60.0f;
    float near_plane = 0.1f;
    float far_plane  = 1000.0f;

    Camera() { update_vectors(); }

    Mat4 view_matrix() const {
        return Mat4::look_at(position, position + front_, up_);
    }

    Mat4 projection_matrix(float aspect) const {
        return Mat4::perspective(math::radians(fov), aspect, near_plane, far_plane);
    }

    Mat4 view_projection(float aspect) const {
        return projection_matrix(aspect) * view_matrix();
    }

    void update_vectors() {
        float yaw_rad = math::radians(yaw);
        float pitch_rad = math::radians(pitch);
        front_.x = std::cos(yaw_rad) * std::cos(pitch_rad);
        front_.y = std::sin(pitch_rad);
        front_.z = std::sin(yaw_rad) * std::cos(pitch_rad);
        front_ = front_.normalized();
        right_ = front_.cross(world_up_).normalized();
        up_    = right_.cross(front_).normalized();
    }

    // Movement
    void move_forward(float amount)  { position += front_ * amount; }
    void move_backward(float amount) { position -= front_ * amount; }
    void move_left(float amount)     { position -= right_ * amount; }
    void move_right(float amount)    { position += right_ * amount; }
    void move_up(float amount)       { position += world_up_ * amount; }
    void move_down(float amount)     { position -= world_up_ * amount; }

    void look(float dx, float dy, float sensitivity = 0.1f) {
        yaw   += dx * sensitivity;
        pitch += dy * sensitivity;
        pitch = math::clamp(pitch, -89.0f, 89.0f);
        update_vectors();
    }

    Vec3 front() const { return front_; }
    Vec3 right() const { return right_; }
    Vec3 up() const { return up_; }

private:
    Vec3 front_ = {0, 0, -1};
    Vec3 up_    = {0, 1, 0};
    Vec3 right_ = {1, 0, 0};
    Vec3 world_up_ = {0, 1, 0};
};

class Camera2D {
public:
    Vec2 position = {0, 0};
    float zoom = 1.0f;
    float rotation = 0.0f; // radians

    Mat4 view_matrix() const {
        Mat4 t = Mat4::translation({-position.x, -position.y, 0});
        Mat4 r = Mat4::rotation_z(-rotation);
        Mat4 s = Mat4::scale(zoom);
        return s * r * t;
    }

    Mat4 projection_matrix(float width, float height) const {
        return Mat4::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, -1.0f, 1.0f);
    }

    Mat4 view_projection(float width, float height) const {
        return projection_matrix(width, height) * view_matrix();
    }
};

} // namespace tgl

#endif // TGL_GRAPHICS_CAMERA_H
