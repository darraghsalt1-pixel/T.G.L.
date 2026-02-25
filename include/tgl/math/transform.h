#ifndef TGL_MATH_TRANSFORM_H
#define TGL_MATH_TRANSFORM_H

#include "vec3.h"
#include "mat4.h"

namespace tgl {

struct Transform {
    Vec3 position = Vec3::zero();
    Vec3 rotation = Vec3::zero(); // Euler angles in radians
    Vec3 scale_   = Vec3::one();

    Mat4 matrix() const {
        return Mat4::translation(position) *
               Mat4::rotation_z(rotation.z) *
               Mat4::rotation_y(rotation.y) *
               Mat4::rotation_x(rotation.x) *
               Mat4::scale(scale_);
    }

    Vec3 forward() const {
        return matrix().transform_direction(Vec3::forward()).normalized();
    }

    Vec3 right() const {
        return matrix().transform_direction(Vec3::right()).normalized();
    }

    Vec3 up() const {
        return matrix().transform_direction(Vec3::up()).normalized();
    }

    void translate(const Vec3& offset) { position += offset; }
    void rotate(const Vec3& euler) { rotation += euler; }
    void set_scale(const Vec3& s) { scale_ = s; }
    void set_scale(float s) { scale_ = Vec3(s); }
};

} // namespace tgl

#endif // TGL_MATH_TRANSFORM_H
