#pragma once

#include <cmath>

namespace ll {

struct Vector4;

inline Vector4 operator-(const Vector4& v);
inline Vector4 operator+(const Vector4& v1, const Vector4& v2);
inline Vector4 operator-(const Vector4& v1, const Vector4& v2);
inline Vector4 operator*(float f, const Vector4& v);
inline float dot(const Vector4& v1, const Vector4& v2);
inline Vector4 cross(const Vector4& v1, const Vector4& v2);

struct Vector4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float data[4];
    };

    static Vector4 zero() {
        return {0, 0, 0, 0};
    }

    static Vector4 position(float x, float y, float z) {
        return {x, y, z, 1};
    }

    static Vector4 direction(float x, float y, float z) {
        return {x, y, z, 0 };
    }

    float normal2() const {
        return x*x + y*y + z*z;
    }

    float normal() const {
        return sqrtf(normal2());
    }

    Vector4 normalized() const {
        float n = normal();
        if (n == 0) {
            return zero();
        }
        return (1/n) * (*this);
    }

    Vector4 asDirection() const {
        return direction(x, y, z);
    }

    Vector4 asPosition() const {
        return position(x, y, z);
    }

    Vector4 toHomogenous() const {
        if (w != 0) {
            return (1/w) * (*this);
        }
        return (*this);
    }
};

inline Vector4 operator-(const Vector4& v) {
    return {-v.x, -v.y, -v.z, -v.w};
}

inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

inline Vector4 operator-(const Vector4& v1, const Vector4& v2) {
    return v1 + (-v2);
}

inline Vector4 operator*(float f, const Vector4& v) {
    return {f * v.x, f * v.y, f * v.z, f * v.w};
}

inline float dot(const Vector4& v1, const Vector4& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline Vector4 cross(const Vector4& v1, const Vector4& v2) {
    auto subCross = [&] (int i, int j) {
        return v1.data[i] * v2.data[j] - v1.data[j] * v2.data[i];
    };
    return {subCross(1, 2), subCross(2, 0), subCross(0, 1)};
}

}
