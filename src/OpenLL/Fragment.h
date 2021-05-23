#pragma once

#include "Vector2.h"
#include "Vector4.h"
#include "Color.h"

namespace ll {

struct Fragment {
    int x;
    int y;
    float z;
    Vector4 world;
    Vector2 uv;
    Color color;
};

}
