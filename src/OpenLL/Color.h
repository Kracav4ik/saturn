#pragma once

namespace ll {

struct Color {
    Color(float a, float r, float g, float b);
    Color(float r, float g, float b);
    Color();

    float a;
    float r;
    float g;
    float b;

    bool isDiscard() const;

    static const Color DISCARD;
};

inline Color operator+(const Color& c1, const Color& c2) {
    return Color(c1.a + c2.a, c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}

inline Color operator*(float f, const Color& c) {
    return Color(f * c.a, f * c.r, f * c.g, f * c.b);
}

}
