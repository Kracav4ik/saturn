#pragma once

#include "Triangle.h"
#include "Matrix4x4.h"
#include "Sampler.h"
#include "Shader.h"

#include <vector>
#include <stack>
#include <memory>

namespace ll {

class Framebuffer;

enum class CullMode {
    DrawCW,
    DrawCCW,
    DrawBoth,
};

struct DrawCall {
    template<typename T>
    DrawCall(const std::vector<T>& vec, Shader shader, Matrix4x4 transform);

    DrawCall();

    std::vector<std::unique_ptr<Shape>> objects;
    Shader shader;
    Matrix4x4 transform;
};

class DrawAPI {
    class TransformWrapper {
    friend class DrawAPI;

    public:
        TransformWrapper(const TransformWrapper&) = delete;
        TransformWrapper operator=(const TransformWrapper&) = delete;
        TransformWrapper(TransformWrapper&&) = delete;
        TransformWrapper operator=(TransformWrapper&&) = delete;

        ~TransformWrapper() {
            std::swap(drawApi.stack, savedStack);
        }

    private:
        explicit TransformWrapper(DrawAPI& drawApi)
                : drawApi(drawApi)
                , savedStack(drawApi.stack)
        {
        }

        std::stack<Matrix4x4> savedStack;
        DrawAPI& drawApi;
    };

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

    TransformWrapper saveTransform();

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
