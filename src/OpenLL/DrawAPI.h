#pragma once

#include "Triangle.h"

#include <vector>
#include <functional>

namespace ll {

class Framebuffer;
class Color;

using Shader = std::function<Color(int, int)>;

class DrawAPI {
public:
    void setFragmentShader(Shader shader);

    void reset();

    void clear(Framebuffer& fb, const Color& color) const;

    void addTriangles(const std::vector<Triangle>& triangles);

    void drawFrame(Framebuffer& fb) const;

private:
    Shader fragmentShader;
    std::vector<Triangle> objects;
};

}
