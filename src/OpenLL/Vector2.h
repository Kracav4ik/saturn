#pragma once

namespace ll {

struct Vector2 {
    float u;
    float v;
};

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    return {v1.u + v2.u, v1.v + v2.v};
}

inline Vector2 operator*(float f, const Vector2& v) {
    return {f * v.u, f * v.v};
}

}
