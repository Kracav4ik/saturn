#pragma once

#include <vector>

namespace ll {

class Framebuffer;
class Color;

class DrawAPI {
public:
    static void clear(Framebuffer& fb, const Color& color);
};

}
