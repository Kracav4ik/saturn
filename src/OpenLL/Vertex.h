#pragma once

#include "Color.h"
#include "Vector2.h"
#include "Vector4.h"

namespace ll {

struct Vertex {
    Color color;
    Vector4 pos;
    Vector2 uv;
};

}
