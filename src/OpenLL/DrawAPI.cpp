#include "DrawAPI.h"
#include "Triangle.h"
#include "Framebuffer.h"
#include "Fragment.h"

using namespace ll;

Matrices::Matrices(const DrawAPI& drawApi)
    : toScreen(drawApi.getToScreenMatrix())
    , viewProjection(drawApi.getViewProjectionMatrix())
    , model(drawApi.getModelMatrix())
    , fullTransform(toScreen * viewProjection * model)
{
}

Matrices::Matrices()
    : toScreen(Matrix4x4::zero())
    , viewProjection(Matrix4x4::zero())
    , model(Matrix4x4::zero())
    , fullTransform(Matrix4x4::zero())
{
}

DrawAPI::DrawAPI() {
    reset();
}

void DrawAPI::setFragmentShader(Shader shader) {
    fragmentShader = std::move(shader);
}

void DrawAPI::setCullMode(CullMode cullMode) {
    cull = cullMode;
}

void DrawAPI::reset() {
    drawCalls.clear();
    loadModelIdentity();
    toScreenMatrix = Matrix4x4::identity();
    viewProjectionMatrix = Matrix4x4::identity();
}

void DrawAPI::clear(Framebuffer& fb, const Color& color) const {
    std::fill(fb.colors.begin(), fb.colors.end(), color);
    std::fill(fb.zOrder.begin(), fb.zOrder.end(), 10);
}

void DrawAPI::drawRound(const Vertex& center, float radius, bool isSolid) {
    std::vector<Line> lines;

    float step = M_PI / 64 / radius;
    Vertex start{center.color, center.pos + radius * Vector4{1, 0, 0}};
    Vertex from = start;
    float angle = step;

    while (angle < M_PI * 2) {
        Vertex to{center.color, center.pos + radius * Vector4{cosf(angle), sinf(angle), 0}};
        int angleDeg = angle * 180 / M_PI;
        if (isSolid || (angleDeg % 45 < 15)) {
            lines.emplace_back(from, to);
        }

        from = to;
        angle += step;
    }

    lines.emplace_back(from, start);

    drawCalls.emplace_back(lines, fragmentShader, Matrices(*this), cull);
}

void DrawAPI::drawLinesCube(const Vector4& center, float size, Color color) {
    auto vexes = getCubeVexes(center, size);

    addShapes<ll::Line>( {
            { {color, vexes[0]}, {color, vexes[1]} },
            { {color, vexes[2]}, {color, vexes[3]} },
            { {color, vexes[4]}, {color, vexes[5]} },
            { {color, vexes[6]}, {color, vexes[7]} },
            { {color, vexes[0]}, {color, vexes[4]} },
            { {color, vexes[1]}, {color, vexes[5]} },
            { {color, vexes[2]}, {color, vexes[6]} },
            { {color, vexes[3]}, {color, vexes[7]} },
            { {color, vexes[0]}, {color, vexes[2]} },
            { {color, vexes[1]}, {color, vexes[3]} },
            { {color, vexes[4]}, {color, vexes[6]} },
            { {color, vexes[5]}, {color, vexes[7]} },
    });
}

void DrawAPI::drawCube(const Vector4& center, float size, Color color) {
    auto vexes = getCubeVexes(center, size);

    auto getTrianglesFromRect = [&](ll::Vertex lb, ll::Vertex lt, ll::Vertex rt, ll::Vertex rb) {
        return std::vector<ll::Triangle> {
                ll::Triangle{ rt, lt, lb },
                ll::Triangle{ rt, rb, lt },
        };
    };

    addShapes(getTrianglesFromRect(
            {color, vexes[2]},
            {color, vexes[6]},
            {color, vexes[0]},
            {color, vexes[4]}
    ));
    addShapes(getTrianglesFromRect(
            {color, vexes[0]},
            {color, vexes[4]},
            {color, vexes[1]},
            {color, vexes[5]}
    ));
    addShapes(getTrianglesFromRect(
            {color, vexes[0]},
            {color, vexes[1]},
            {color, vexes[2]},
            {color, vexes[3]}
    ));
    addShapes(getTrianglesFromRect(
            {color, vexes[3]},
            {color, vexes[7]},
            {color, vexes[2]},
            {color, vexes[6]}
    ));
    addShapes(getTrianglesFromRect(
            {color, vexes[1]},
            {color, vexes[5]},
            {color, vexes[3]},
            {color, vexes[7]}
    ));
    addShapes(getTrianglesFromRect(
            {color,vexes[6]},
            {color,vexes[7]},
            {color,vexes[4]},
            {color,vexes[5]}
    ));
}
void DrawAPI::drawSphere(const Vertex& center, float radius) {
    for (float i = 0; i < radius; i += radius / 10) {
        drawRound({center.color, center.pos - Vector4::direction(0, 0, radius - i)}, i, true);
        drawRound({center.color, center.pos + Vector4::direction(0, 0, radius - i)}, i, true);
    }
}

void DrawAPI::drawFrame(Framebuffer& fb) const {
    for (const auto& drawCall : drawCalls) {
        for (const auto& object : drawCall.objects) {
            for (const auto& frag : object->getFragments(fb, drawCall.matrices, drawCall.cull)) {
                if (frag.z < -1 || frag.z > 1) {
                    continue;
                }
                fb.putPixel(frag.x, frag.y, frag.z, drawCall.shader(frag, sampler.get()));
            }
        }
    }
}

void DrawAPI::loadModelIdentity() {
    auto s = std::stack<Matrix4x4>();
    modelStack.swap(s);
}

void DrawAPI::pushModelMatrix(const Matrix4x4& mat) {
    modelStack.push(getModelMatrix() * mat);
}

void DrawAPI::popModelMatrix() {
    modelStack.pop();
}

Matrix4x4 DrawAPI::getModelMatrix() const {
    if (modelStack.empty()) {
        return Matrix4x4::identity();
    } else {
        return modelStack.top();
    }
}

void DrawAPI::setToScreenMatrix(const Matrix4x4& mat) {
    toScreenMatrix = mat;
}

Matrix4x4 DrawAPI::getToScreenMatrix() const {
    return toScreenMatrix;
}

void DrawAPI::setViewProjectionMatrix(const Matrix4x4& mat) {
    viewProjectionMatrix = mat;
}

Matrix4x4 DrawAPI::getViewProjectionMatrix() const {
    return viewProjectionMatrix;
}

void DrawAPI::loadTexture(const uint32_t* data, int width, int height) {
    sampler = std::make_unique<Sampler>(data, width, height);
}

DrawAPI::TransformWrapper DrawAPI::saveTransform() {
    return DrawAPI::TransformWrapper(*this);
}

std::vector<ll::Vector4> DrawAPI::getCubeVexes(const Vector4& center, float size) {
    return {
        center + size * ll::Vector4::direction(-0.5, -0.5, -0.5),
        center + size * ll::Vector4::direction(-0.5, -0.5, 0.5),
        center + size * ll::Vector4::direction(-0.5, 0.5, -0.5),
        center + size * ll::Vector4::direction(-0.5, 0.5, 0.5),
        center + size * ll::Vector4::direction(0.5, -0.5, -0.5),
        center + size * ll::Vector4::direction(0.5, -0.5, 0.5),
        center + size * ll::Vector4::direction(0.5, 0.5, -0.5),
        center + size * ll::Vector4::direction(0.5, 0.5, 0.5),
    };
}
