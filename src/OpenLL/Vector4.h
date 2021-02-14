#pragma once

namespace ll {

struct Vector4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float t;

        };
        float data[4];
    };

    static Vector4 zero() {
        return {0, 0, 0, 0};
    }

    static Vector4 position(const Vector4& v) {
        return {v.x, v.y, v.z, 1};
    }

    static Vector4 direction(const Vector4& v) {
        return {v.x, v.y, v.z, 0 };
    }
};

inline Vector4 operator-(const Vector4& v) {
    return {-v.x, -v.y, -v.z, -v.t};
}

inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.t + v2.t};
}

inline Vector4 operator-(const Vector4& v1, const Vector4& v2) {
    return v1 + (-v2);
}

inline Vector4 operator*(float f, const Vector4& v) {
    return {f * v.x, f * v.y, f * v.z, f * v.t};
}

inline float dot(const Vector4& v1, const Vector4& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.t * v2.t;
}

}
