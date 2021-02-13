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
};

}
