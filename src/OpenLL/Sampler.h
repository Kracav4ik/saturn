#pragma once

#include "Color.h"
#include <vector>
#include <string>

namespace ll {

class Vector2;
class Sampler {
public:
    Sampler(const uint32_t* data, int width, int height);
    Color getColor(const Vector2& uv) const;

private:
    std::vector<Color> img;

    int w;
    int h;
};

}
