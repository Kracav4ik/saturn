#pragma once

#include "Triangle.h"
#include "Matrix4x4.h"

#include <vector>
#include <functional>
#include <stack>
#include <memory>

namespace ll {

class Framebuffer;
class Color;

class Sampler;

using Shader = std::function<Color(const Vertex&, const Sampler*)>;

enum class CullMode {
    DrawCW,
    DrawCCW,
    DrawBoth,
};

struct DrawCall {
    std::vector<Triangle> objects;
    Shader shader;
    Matrix4x4 transform;
};

class DrawAPI {
public:
    void setFragmentShader(Shader shader);
    void setCullMode(CullMode cullMode);

    void reset();

    void clear(Framebuffer& fb, const Color& color) const;

    void addTriangles(const std::vector<Triangle>& triangles);

    void drawFrame(Framebuffer& fb) const;

    void loadIdentity();
    void pushMatrix(const Matrix4x4& mat);
    void popMatrix();
    void loadTexture(const uint32_t* data, int width, int height);

private:
    Matrix4x4 getMatrix() const;
    void processFrags(Framebuffer& fb, const Triangle& triangle, const Shader& shader, const Matrix4x4& transform) const;

    std::unique_ptr<Sampler> sampler;
    Shader fragmentShader;
    CullMode cull = CullMode::DrawCW;
    std::vector<DrawCall> drawCalls;
    std::stack<Matrix4x4> stack;
};

}
