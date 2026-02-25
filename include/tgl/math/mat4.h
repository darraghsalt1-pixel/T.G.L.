#ifndef TGL_MATH_MAT4_H
#define TGL_MATH_MAT4_H

#include "vec3.h"
#include "vec4.h"
#include <cmath>
#include <cstring>

namespace tgl {

// Column-major 4x4 matrix (OpenGL convention)
struct Mat4 {
    float m[16] = {};

    Mat4() { identity(); }

    void identity() {
        std::memset(m, 0, sizeof(m));
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    float& operator()(int row, int col) { return m[col * 4 + row]; }
    float operator()(int row, int col) const { return m[col * 4 + row]; }
    const float* data() const { return m; }
    float* data() { return m; }

    Mat4 operator*(const Mat4& b) const {
        Mat4 result;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                result.m[col * 4 + row] =
                    m[0 * 4 + row] * b.m[col * 4 + 0] +
                    m[1 * 4 + row] * b.m[col * 4 + 1] +
                    m[2 * 4 + row] * b.m[col * 4 + 2] +
                    m[3 * 4 + row] * b.m[col * 4 + 3];
            }
        }
        return result;
    }

    Vec4 operator*(const Vec4& v) const {
        return {
            m[0]*v.x + m[4]*v.y + m[8]*v.z  + m[12]*v.w,
            m[1]*v.x + m[5]*v.y + m[9]*v.z  + m[13]*v.w,
            m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w,
            m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w
        };
    }

    Vec3 transform_point(const Vec3& p) const {
        Vec4 r = *this * Vec4(p, 1.0f);
        return r.xyz() / r.w;
    }

    Vec3 transform_direction(const Vec3& d) const {
        Vec4 r = *this * Vec4(d, 0.0f);
        return r.xyz();
    }

    Mat4 transposed() const {
        Mat4 r;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                r.m[j * 4 + i] = m[i * 4 + j];
        return r;
    }

    Mat4 inversed() const {
        Mat4 inv;
        float* o = inv.m;
        const float* a = m;

        float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3];
        float a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7];
        float a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11];
        float a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

        float b00 = a00*a11 - a01*a10, b01 = a00*a12 - a02*a10;
        float b02 = a00*a13 - a03*a10, b03 = a01*a12 - a02*a11;
        float b04 = a01*a13 - a03*a11, b05 = a02*a13 - a03*a12;
        float b06 = a20*a31 - a21*a30, b07 = a20*a32 - a22*a30;
        float b08 = a20*a33 - a23*a30, b09 = a21*a32 - a22*a31;
        float b10 = a21*a33 - a23*a31, b11 = a22*a33 - a23*a32;

        float det = b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06;
        if (std::abs(det) < 1e-8f) return Mat4{};
        float inv_det = 1.0f / det;

        o[0]  = ( a11*b11 - a12*b10 + a13*b09) * inv_det;
        o[1]  = (-a01*b11 + a02*b10 - a03*b09) * inv_det;
        o[2]  = ( a31*b05 - a32*b04 + a33*b03) * inv_det;
        o[3]  = (-a21*b05 + a22*b04 - a23*b03) * inv_det;
        o[4]  = (-a10*b11 + a12*b08 - a13*b07) * inv_det;
        o[5]  = ( a00*b11 - a02*b08 + a03*b07) * inv_det;
        o[6]  = (-a30*b05 + a32*b02 - a33*b01) * inv_det;
        o[7]  = ( a20*b05 - a22*b02 + a23*b01) * inv_det;
        o[8]  = ( a10*b10 - a11*b08 + a13*b06) * inv_det;
        o[9]  = (-a00*b10 + a01*b08 - a03*b06) * inv_det;
        o[10] = ( a30*b04 - a31*b02 + a33*b00) * inv_det;
        o[11] = (-a20*b04 + a21*b02 - a23*b00) * inv_det;
        o[12] = (-a10*b09 + a11*b07 - a12*b06) * inv_det;
        o[13] = ( a00*b09 - a01*b07 + a02*b06) * inv_det;
        o[14] = (-a30*b03 + a31*b01 - a32*b00) * inv_det;
        o[15] = ( a20*b03 - a21*b01 + a22*b00) * inv_det;

        return inv;
    }

    // Factory methods
    static Mat4 translation(const Vec3& t) {
        Mat4 r;
        r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z;
        return r;
    }

    static Mat4 scale(const Vec3& s) {
        Mat4 r;
        r.m[0] = s.x; r.m[5] = s.y; r.m[10] = s.z;
        return r;
    }

    static Mat4 scale(float s) { return scale({s, s, s}); }

    static Mat4 rotation_x(float rad) {
        Mat4 r;
        float c = std::cos(rad), s = std::sin(rad);
        r.m[5] = c;  r.m[6] = s;
        r.m[9] = -s; r.m[10] = c;
        return r;
    }

    static Mat4 rotation_y(float rad) {
        Mat4 r;
        float c = std::cos(rad), s = std::sin(rad);
        r.m[0] = c;  r.m[2] = -s;
        r.m[8] = s;  r.m[10] = c;
        return r;
    }

    static Mat4 rotation_z(float rad) {
        Mat4 r;
        float c = std::cos(rad), s = std::sin(rad);
        r.m[0] = c;  r.m[1] = s;
        r.m[4] = -s; r.m[5] = c;
        return r;
    }

    static Mat4 rotation(const Vec3& axis, float rad) {
        Vec3 a = axis.normalized();
        float c = std::cos(rad), s = std::sin(rad), t = 1.0f - c;
        Mat4 r;
        r.m[0]  = t*a.x*a.x + c;       r.m[1]  = t*a.x*a.y + s*a.z; r.m[2]  = t*a.x*a.z - s*a.y;
        r.m[4]  = t*a.x*a.y - s*a.z;   r.m[5]  = t*a.y*a.y + c;     r.m[6]  = t*a.y*a.z + s*a.x;
        r.m[8]  = t*a.x*a.z + s*a.y;   r.m[9]  = t*a.y*a.z - s*a.x; r.m[10] = t*a.z*a.z + c;
        return r;
    }

    static Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = f.cross(up.normalized()).normalized();
        Vec3 u = s.cross(f);
        Mat4 r;
        r.m[0] = s.x;   r.m[4] = s.y;   r.m[8]  = s.z;   r.m[12] = -s.dot(eye);
        r.m[1] = u.x;   r.m[5] = u.y;   r.m[9]  = u.z;   r.m[13] = -u.dot(eye);
        r.m[2] = -f.x;  r.m[6] = -f.y;  r.m[10] = -f.z;  r.m[14] = f.dot(eye);
        r.m[3] = 0;     r.m[7] = 0;     r.m[11] = 0;     r.m[15] = 1;
        return r;
    }

    static Mat4 perspective(float fov_rad, float aspect, float near, float far) {
        float f = 1.0f / std::tan(fov_rad * 0.5f);
        Mat4 r;
        std::memset(r.m, 0, sizeof(r.m));
        r.m[0]  = f / aspect;
        r.m[5]  = f;
        r.m[10] = (far + near) / (near - far);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * far * near) / (near - far);
        return r;
    }

    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
        Mat4 r;
        std::memset(r.m, 0, sizeof(r.m));
        r.m[0]  = 2.0f / (right - left);
        r.m[5]  = 2.0f / (top - bottom);
        r.m[10] = -2.0f / (far - near);
        r.m[12] = -(right + left) / (right - left);
        r.m[13] = -(top + bottom) / (top - bottom);
        r.m[14] = -(far + near) / (far - near);
        r.m[15] = 1.0f;
        return r;
    }
};

} // namespace tgl

#endif // TGL_MATH_MAT4_H
