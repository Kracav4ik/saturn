#include "DrawAPI.h"
#include "Framebuffer.h"
#include "Fragment.h"

using namespace ll;

template<typename T>
DrawCall::DrawCall(const std::vector<T>& vec, Shader shader, Matrix4x4 transform, CullMode cull)
    : shader(std::move(shader))
    , transform(std::move(transform))
    , cull(cull)
{
    objects.reserve(vec.size());
    for (const auto& item : vec) {
        objects.template emplace_back(new T(item));
    }
}

DrawCall::DrawCall()
    : transform(Matrix4x4::zero())
{
}

void DrawAPI::setFragmentShader(Shader shader) {
    fragmentShader = std::move(shader);
}

void DrawAPI::setCullMode(CullMode cullMode) {
    cull = cullMode;
}

void DrawAPI::reset() {
    drawCalls.clear();
    loadIdentity();
}

void DrawAPI::clear(Framebuffer& fb, const Color& color) const {
    std::fill(fb.colors.begin(), fb.colors.end(), color);
    std::fill(fb.zOrder.begin(), fb.zOrder.end(), 10);
}

void DrawAPI::addLines(const std::vector<Line>& lines) {
    drawCalls.emplace_back(lines, fragmentShader, getMatrix(), cull);
}

void DrawAPI::drawRound(const Vertex& center, float radius, bool isDash) {
    bool isDrawCur = true;
    std::vector<Line> lines;

    float step = M_PI / 64 / radius;
    Vertex start{center.color, center.pos + radius * Vector4{1, 0, 0}};
    Vertex from = start;
    float angle = step;

    while (angle < M_PI * 2) {
        Vertex to{center.color, center.pos + radius * Vector4{cosf(angle), sinf(angle), 0}};
        if (isDash || isDrawCur) {
            lines.emplace_back(from, to);
        }

        isDrawCur = !isDrawCur;
        from = to;
        angle += step;
    }

    lines.emplace_back(from, start);

    drawCalls.emplace_back(lines, fragmentShader, getMatrix(), cull);
}

void DrawAPI::addTriangles(const std::vector<Triangle>& triangles) {
    drawCalls.emplace_back(triangles, fragmentShader, getMatrix(), cull);
}

void DrawAPI::drawFrame(Framebuffer& fb) const {
    for (const auto& drawCall : drawCalls) {
        for (const auto& object : drawCall.objects) {
            for (const auto& frag : object->getFragments(fb, drawCall.transform, drawCall.cull)) {
                fb.putPixel(frag.x, frag.y, frag.z, drawCall.shader(frag, sampler.get()));
            }
        }
    }
}

void DrawAPI::loadIdentity() {
    auto s = std::stack<Matrix4x4>();
    stack.swap(s);
}

void DrawAPI::pushMatrix(const Matrix4x4& mat) {
    stack.push(getMatrix() * mat);
}

void DrawAPI::popMatrix() {
    stack.pop();
}

void DrawAPI::loadTexture(const uint32_t* data, int width, int height) {
    sampler = std::make_unique<Sampler>(data, width, height);
}

Matrix4x4 DrawAPI::getMatrix() const {
    if (stack.empty()) {
        return Matrix4x4::identity();
    } else {
        return stack.top();
    }
}

void DrawAPI::processFrags(Framebuffer& fb, const Triangle& triangle, const Shader& shader, const Matrix4x4& transform) const {
}

DrawAPI::TransformWrapper DrawAPI::saveTransform() {
    return DrawAPI::TransformWrapper(*this);
}

