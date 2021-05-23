#pragma once

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
class DrawAPI;

enum class CullMode {
    DrawCW,
    DrawCCW,
    DrawBoth,
};

struct Matrices {
    explicit Matrices(const DrawAPI& drawApi);
    Matrices();

    Matrix4x4 toScreen;
    Matrix4x4 viewProjection;
    Matrix4x4 model;
    Matrix4x4 fullTransform;
};

struct DrawCall {
    template<typename T>
    DrawCall(const std::vector<T>& vec, Shader shader, const Matrices& matrices, CullMode cull)
        : shader(std::move(shader))
        , matrices(matrices)
        , cull(cull)
    {
        objects.reserve(vec.size());
        for (const auto& item : vec) {
            objects.template emplace_back(new T(item));
        }
    }

    DrawCall() = default;

    std::vector<std::unique_ptr<Shape>> objects;
    Shader shader;
    Matrices matrices;
    CullMode cull = CullMode::DrawBoth;
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
            std::swap(drawApi.modelStack, savedStack);
            drawApi.toScreenMatrix = savedToScreen;
            drawApi.viewProjectionMatrix = savedViewProjection;
        }

    private:
        explicit TransformWrapper(DrawAPI& drawApi)
                : drawApi(drawApi)
                , savedStack(drawApi.modelStack)
                , savedToScreen(drawApi.toScreenMatrix)
                , savedViewProjection(drawApi.viewProjectionMatrix)
        {
        }

        DrawAPI& drawApi;
        std::stack<Matrix4x4> savedStack;
        Matrix4x4 savedToScreen;
        Matrix4x4 savedViewProjection;
    };

public:
    DrawAPI();

    void setFragmentShader(Shader shader);
    void setCullMode(CullMode cullMode);

    void reset();

    void clear(Framebuffer& fb, const Color& color) const;

    template<
        typename T,
        typename = std::enable_if_t<std::is_base_of_v<Shape, T>>
    >
    void addShapes(const std::vector<T>& shapes) {
        drawCalls.emplace_back(shapes, fragmentShader, Matrices(*this), cull);
    }
    void drawRound(const Vertex& center, float radius, bool isSolid);
    void drawLinesCube(const Vector4& center, float size, Color color);
    void drawCube(const Vector4& center, float size, Color color);
    void drawSphere(const Vertex& center, float radius);

    void drawFrame(Framebuffer& fb) const;

    void loadModelIdentity();
    void pushModelMatrix(const Matrix4x4& mat);
    void popModelMatrix();
    Matrix4x4 getModelMatrix() const;

    void setToScreenMatrix(const Matrix4x4& mat);
    Matrix4x4 getToScreenMatrix() const;

    void setViewProjectionMatrix(const Matrix4x4& mat);
    Matrix4x4 getViewProjectionMatrix() const;

    void loadTexture(const uint32_t* data, int width, int height);

    TransformWrapper saveTransform();
    static std::vector<ll::Vector4> getCubeVexes(const Vector4& center, float size);

private:
    std::unique_ptr<Sampler> sampler;
    Shader fragmentShader;
    CullMode cull = CullMode::DrawBoth;
    std::vector<DrawCall> drawCalls;
    std::stack<Matrix4x4> modelStack;
    Matrix4x4 toScreenMatrix;
    Matrix4x4 viewProjectionMatrix;
};

}
