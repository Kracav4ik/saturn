#pragma once

#include "Vector4.h"

namespace ll {

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
};

inline Matrix4x4 operator-(const Matrix4x4& m) {
    return {-m.rows[0], -m.rows[1], -m.rows[2], -m.rows[3]};
}

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
    return {m1.rows[0] + m2.rows[0], m1.rows[1] + m2.rows[1], m1.rows[2] + m2.rows[2], m1.rows[3] + m2.rows[3]};
}

inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
    return m1 + (-m2);
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
