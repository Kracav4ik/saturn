#include "Framebuffer.h"

using namespace ll;

Framebuffer::Framebuffer(int w, int h)
    : w(w)
    , h(h)
    , colors(w * h)
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

Color& Framebuffer::at(int x, int y) {
    return colors[y * w + x];
}
