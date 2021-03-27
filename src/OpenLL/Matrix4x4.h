#pragma once

#include <cmath>

#include "Vector4.h"

namespace ll {

struct Matrix4x4;

inline Matrix4x4 operator-(const Matrix4x4& m);
inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator*(float f, const Matrix4x4& m);
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
inline Vector4 operator*(const Matrix4x4& m, const Vector4& v);

struct Matrix4x4 {
    union {
        Vector4 rows[4];
        float data[16];
    };

    Vector4 col(int i) const {
        return {rows[0].data[i], rows[1].data[i], rows[2].data[i], rows[3].data[i]};
    }

    static Matrix4x4 zero() {
        return {Vector4::zero(), Vector4::zero(), Vector4::zero(), Vector4::zero()};
    }

    static Matrix4x4 identity() {
        return {
            Vector4{1, 0, 0, 0},
            Vector4{0, 1, 0, 0},
            Vector4{0, 0, 1, 0},
            Vector4{0, 0, 0, 1},
        };
    }

    static Matrix4x4 rotX(float angle) {
        Matrix4x4 ret = identity();
        ret.rows[1].data[1] = std::cos(angle);
        ret.rows[2].data[2] = std::cos(angle);
        ret.rows[2].data[1] = std::sin(angle);
        ret.rows[1].data[2] = -std::sin(angle);
        return ret;
    }

    static Matrix4x4 rotY(float angle) {
        Matrix4x4 ret = identity();
        ret.rows[0].data[0] = std::cos(angle);
        ret.rows[2].data[2] = std::cos(angle);
        ret.rows[2].data[0] = -std::sin(angle);
        ret.rows[0].data[2] = std::sin(angle);
        return ret;
    }

    static Matrix4x4 rotZ(float angle) {
        Matrix4x4 ret = identity();
        ret.rows[0].data[0] = std::cos(angle);
        ret.rows[1].data[1] = std::cos(angle);
        ret.rows[1].data[0] = std::sin(angle);
        ret.rows[0].data[1] = -std::sin(angle);
        return ret;
    }

    static Matrix4x4 translation(const Vector4& v) {
        Matrix4x4 ret = identity();
        for (int i = 0; i < 3; ++i) {
            ret.rows[i].data[3] = v.data[i];
        }
        return ret;
    }

    static Matrix4x4 translation(float x, float y, float z) {
        return translation(Vector4::direction(x, y, z));
    }

    static Matrix4x4 scale(float x, float y, float z) {
        return scale(Vector4::direction(x, y, z));
    }

    static Matrix4x4 scale(float f) {
        return scale(f, f, f);
    }

    static Matrix4x4 scale(const Vector4& v) {
        Matrix4x4 ret = identity();
        for (int i = 0; i < 3; ++i) {
            ret.rows[i].data[i] = v.data[i];
        }
        return ret;
    }

    static Matrix4x4 lookAt(const Vector4& camera, const Vector4& target, const Vector4& upVector) {
        Vector4 f = (camera - target).normalized().asDirection();
        Vector4 l = cross(upVector, f).normalized().asDirection();
        Vector4 u = cross(f, l).asDirection();
        Matrix4x4 rot{
                l,
                u,
                f,
                Vector4{0, 0, 0, 1},
        };
        return rot * translation(-camera);
    }

    static Matrix4x4 perspective(float fovY, float aspect, float near, float far) {
        float f = tanf(fovY/2);
        return {
                Vector4{f/aspect, 0, 0, 0},
                Vector4{0, f, 0, 0},
                Vector4{0, 0, -(far + near)/(far - near), -2*far*near/(far - near)},
                Vector4{0, 0, -1, 0},
        };
    }

    static Matrix4x4 frustum(float width, float height, float near, float far) {
        return frustum(width/2, height/2, -width/2, -height/2, near, far);
    }

    static Matrix4x4 frustum(float right, float top, float left, float bottom, float near, float far) {
        return {
                Vector4{2*near/(right - left), 0, (right + left)/(right - left), 0},
                Vector4{0, 2*near/(top - bottom), (top + bottom)/(top - bottom), 0},
                Vector4{0, 0, -(far + near)/(far - near), -2*far*near/(far - near)},
                Vector4{0, 0, -1, 0},
        };
    }

    static Matrix4x4 ortho(float right, float top, float left, float bottom, float near, float far) {
        return {
                Vector4{2/(right - left), 0, 0, -(right + left)/(right - left)},
                Vector4{0, 2/(top - bottom), 0, -(top + bottom)/(top - bottom)},
                Vector4{0, 0, 2/(far - near), -(far + near)/(far - near)},
                Vector4{0, 0, 0, 1},
        };
    }

    static Matrix4x4 toScreenSpace(float width, float height) {
        return scale(width/2, height/2, 1) * translation(1, 1, 0);
    }

    Matrix4x4 inverse() const {
        // taken from MESA implementation of glueInvertMatrixd by David Moore
        Matrix4x4 inv;
        inv.data[0] =   data[5]*data[10]*data[15] - data[5]*data[11]*data[14] - data[9]*data[6]*data[15]
                      + data[9]*data[7]*data[14] + data[13]*data[6]*data[11] - data[13]*data[7]*data[10];
        inv.data[4] =  -data[4]*data[10]*data[15] + data[4]*data[11]*data[14] + data[8]*data[6]*data[15]
                      - data[8]*data[7]*data[14] - data[12]*data[6]*data[11] + data[12]*data[7]*data[10];
        inv.data[8] =   data[4]*data[9]*data[15] - data[4]*data[11]*data[13] - data[8]*data[5]*data[15]
                      + data[8]*data[7]*data[13] + data[12]*data[5]*data[11] - data[12]*data[7]*data[9];
        inv.data[12] = -data[4]*data[9]*data[14] + data[4]*data[10]*data[13] + data[8]*data[5]*data[14]
                      - data[8]*data[6]*data[13] - data[12]*data[5]*data[10] + data[12]*data[6]*data[9];

        float det = data[0]*inv.data[0] + data[1]*inv.data[4] + data[2]*inv.data[8] + data[3]*inv.data[12];
        if (det == 0) {
            return zero();
        }

        inv.data[1] =  -data[1]*data[10]*data[15] + data[1]*data[11]*data[14] + data[9]*data[2]*data[15]
                      - data[9]*data[3]*data[14] - data[13]*data[2]*data[11] + data[13]*data[3]*data[10];
        inv.data[5] =   data[0]*data[10]*data[15] - data[0]*data[11]*data[14] - data[8]*data[2]*data[15]
                      + data[8]*data[3]*data[14] + data[12]*data[2]*data[11] - data[12]*data[3]*data[10];
        inv.data[9] =  -data[0]*data[9]*data[15] + data[0]*data[11]*data[13] + data[8]*data[1]*data[15]
                      - data[8]*data[3]*data[13] - data[12]*data[1]*data[11] + data[12]*data[3]*data[9];
        inv.data[13] =  data[0]*data[9]*data[14] - data[0]*data[10]*data[13] - data[8]*data[1]*data[14]
                      + data[8]*data[2]*data[13] + data[12]*data[1]*data[10] - data[12]*data[2]*data[9];
        inv.data[2] =   data[1]*data[6]*data[15] - data[1]*data[7]*data[14] - data[5]*data[2]*data[15]
                      + data[5]*data[3]*data[14] + data[13]*data[2]*data[7] - data[13]*data[3]*data[6];
        inv.data[6] =  -data[0]*data[6]*data[15] + data[0]*data[7]*data[14] + data[4]*data[2]*data[15]
                      - data[4]*data[3]*data[14] - data[12]*data[2]*data[7] + data[12]*data[3]*data[6];
        inv.data[10] =  data[0]*data[5]*data[15] - data[0]*data[7]*data[13] - data[4]*data[1]*data[15]
                      + data[4]*data[3]*data[13] + data[12]*data[1]*data[7] - data[12]*data[3]*data[5];
        inv.data[14] = -data[0]*data[5]*data[14] + data[0]*data[6]*data[13] + data[4]*data[1]*data[14]
                      - data[4]*data[2]*data[13] - data[12]*data[1]*data[6] + data[12]*data[2]*data[5];
        inv.data[3] =  -data[1]*data[6]*data[11] + data[1]*data[7]*data[10] + data[5]*data[2]*data[11]
                      - data[5]*data[3]*data[10] - data[9]*data[2]*data[7] + data[9]*data[3]*data[6];
        inv.data[7] =   data[0]*data[6]*data[11] - data[0]*data[7]*data[10] - data[4]*data[2]*data[11]
                      + data[4]*data[3]*data[10] + data[8]*data[2]*data[7] - data[8]*data[3]*data[6];
        inv.data[11] = -data[0]*data[5]*data[11] + data[0]*data[7]*data[9] + data[4]*data[1]*data[11]
                      - data[4]*data[3]*data[9] - data[8]*data[1]*data[7] + data[8]*data[3]*data[5];
        inv.data[15] =  data[0]*data[5]*data[10] - data[0]*data[6]*data[9] - data[4]*data[1]*data[10]
                      + data[4]*data[2]*data[9] + data[8]*data[1]*data[6] - data[8]*data[2]*data[5];

        det = 1.f / det;

        for (float & i : inv.data) {
            i *= det;
        }
        return inv;
    }
};

inline Matrix4x4 operator-(const Matrix4x4& m) {
    return {-m.rows[0], -m.rows[1], -m.rows[2], -m.rows[3]};
}

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
    return {m1.rows[0] + m2.rows[0], m1.rows[1] + m2.rows[1], m1.rows[2] + m2.rows[2], m1.rows[3] + m2.rows[3]};
}

inline Matrix4x4 operator*(float f, const Matrix4x4& m) {
    return {f * m.rows[0], f * m.rows[1], f * m.rows[2], f * m.rows[3]};
}

inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
    return {
        Vector4{dot(m1.rows[0], m2.col(0)), dot(m1.rows[0], m2.col(1)), dot(m1.rows[0], m2.col(2)), dot(m1.rows[0], m2.col(3))},
        Vector4{dot(m1.rows[1], m2.col(0)), dot(m1.rows[1], m2.col(1)), dot(m1.rows[1], m2.col(2)), dot(m1.rows[1], m2.col(3))},
        Vector4{dot(m1.rows[2], m2.col(0)), dot(m1.rows[2], m2.col(1)), dot(m1.rows[2], m2.col(2)), dot(m1.rows[2], m2.col(3))},
        Vector4{dot(m1.rows[3], m2.col(0)), dot(m1.rows[3], m2.col(1)), dot(m1.rows[3], m2.col(2)), dot(m1.rows[3], m2.col(3))},
    };
}

inline Vector4 operator*(const Matrix4x4& m, const Vector4& v) {
    return {dot(m.rows[0], v), dot(m.rows[1], v), dot(m.rows[2], v), dot(m.rows[3], v)};
}

}
