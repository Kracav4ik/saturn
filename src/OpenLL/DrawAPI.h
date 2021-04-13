#pragma once

#include "Triangle.h"
#include "Matrix4x4.h"
#include "Sampler.h"
#include "Shader.h"
#include "Line.h"

#include <type_traits>
#include <vector>
#include <stack>
#include <memory>

namespace ll {

class Framebuffer;
struct ParabolicCurve;
struct BezierSurface;

enum class CullMode {
    DrawCW,
    DrawCCW,
    DrawBoth,
};

struct DrawCall {
    template<typename T>
    DrawCall(const std::vector<T>& vec, Shader shader, Matrix4x4 transform, CullMode cull)
        : shader(std::move(shader))
        , transform(std::move(transform))
        , cull(cull)
    {
        objects.reserve(vec.size());
        for (const auto& item : vec) {
            objects.template emplace_back(new T(item));
        }
    }

    DrawCall();

    std::vector<std::unique_ptr<Shape>> objects;
    Shader shader;
    Matrix4x4 transform;
    CullMode cull;
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

    template<
        typename T,
        typename = std::enable_if_t<std::is_base_of_v<Shape, T>>
    >
    void addShapes(const std::vector<T>& shapes) {
        drawCalls.emplace_back(shapes, fragmentShader, getMatrix(), cull);
    }
    void drawRound(const Vertex& center, float radius, bool isSolid);
    void drawLinesCube(const Vector4& center, float size, Color color);
    void drawSphere(const Vertex& center, float radius);

    void drawFrame(Framebuffer& fb) const;

    void loadIdentity();
    void pushMatrix(const Matrix4x4& mat);
    void popMatrix();
    void loadTexture(const uint32_t* data, int width, int height);

    TransformWrapper saveTransform();

    static std::vector<ll::Vector4> getCubeVexes(const Vector4& center, float size);

private:
    Matrix4x4 getMatrix() const;

    std::unique_ptr<Sampler> sampler;
    Shader fragmentShader;
    CullMode cull = CullMode::DrawBoth;
    std::vector<DrawCall> drawCalls;
    std::stack<Matrix4x4> stack;
};

}
