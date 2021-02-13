#include "DrawAPI.h"
#include "Framebuffer.h"

using namespace ll;

void DrawAPI::clear(Framebuffer& fb, const Color& color) {
    std::fill(fb.colors.begin(), fb.colors.end(), color);
}
