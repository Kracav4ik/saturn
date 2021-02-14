#pragma once

#include <vector>
#include <cstdint>
#include "Color.h"

namespace ll {

class Framebuffer {
    friend class DrawAPI;

public:
    Framebuffer(int w, int h);

    int getW() const;
    int getH() const;

    std::vector<uint32_t> getColorsARGB32() const;

private:
    Color& at(int x, int y);

    int w;
    int h;
    std::vector<Color> colors;
};

}
