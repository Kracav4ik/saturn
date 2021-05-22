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
    float getZ(int x, int y) const;

    std::vector<uint32_t> getColorsARGB32() const;

private:
    void putPixel(int x, int y, float z, Color color);

    int w;
    int h;
    std::vector<Color> colors;
    std::vector<float> zOrder;
};

}
