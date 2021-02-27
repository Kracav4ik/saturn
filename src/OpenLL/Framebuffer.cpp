#include "Framebuffer.h"

using namespace ll;

Framebuffer::Framebuffer(int w, int h)
    : w(w)
    , h(h)
    , colors(w * h)
    , zOrder(colors.size())
{
}

int Framebuffer::getW() const {
    return w;
}

int Framebuffer::getH() const {
    return h;
}

std::vector<uint32_t> Framebuffer::getColorsARGB32() const {
    std::vector<uint32_t> result;
    result.reserve(colors.size());
    for (const auto& c : colors) {
        uint8_t a = c.a * 255;
        uint8_t r = c.r * 255;
        uint8_t g = c.g * 255;
        uint8_t b = c.b * 255;
        result.push_back((a << 24) | (r << 16) | (g << 8) | b);
    }
    return result;
}

void Framebuffer::putPixel(int x, int y, float z, Color color) {
    int idx = (h - 1 - y) * w + x;

    if (0 > x || x >= w || 0 > y || y >= h || zOrder[idx] < z) {
        return;
    }

    colors[idx] = color;
    zOrder[idx] = z;
}
