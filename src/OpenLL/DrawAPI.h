#pragma once

#include "Triangle.h"
#include "Matrix4x4.h"

#include <vector>
#include <functional>
#include <stack>

namespace ll {

class Framebuffer;
class Color;

using Shader = std::function<Color(float, float)>;

class DrawAPI {
public:
    void setFragmentShader(Shader shader);

    void reset();

    void clear(Framebuffer& fb, const Color& color) const;

    void addTriangles(const std::vector<Triangle>& triangles);

    void drawFrame(Framebuffer& fb) const;

    void loadIdentity();
    void pushMatrix(const Matrix4x4& mat);
    void popMatrix();

private:
    Matrix4x4 getMatrix() const;

    Shader fragmentShader;
    std::vector<Triangle> objects;
    std::stack<Matrix4x4> stack;
};

}
