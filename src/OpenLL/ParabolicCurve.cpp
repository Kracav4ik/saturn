#include "ParabolicCurve.h"

#include "Framebuffer.h"
#include "Fragment.h"
#include "Matrix4x4.h"

using namespace ll;

ParabolicCurve::ParabolicCurve(std::vector<Vector4> vertexes, bool solidColor, Color color)
    : vertexes(std::move(vertexes))
    , solidColor(solidColor)
    , color(color)
{}

std::vector<Fragment> ParabolicCurve::getFragments(Framebuffer& fb, const Matrix4x4& transform, CullMode cull) const {
    auto frags = std::vector<Fragment>();

    auto Pt = [](float  t, const Vector4& p1, const Vector4& p2, const Vector4& p3) {
        return + .5f * t * (t - 1) * p1
               - (t - 1) * (t + 1) * p2
               + .5f * t * (t + 1) * p3;
    };

    std::vector<Color> colors;
    if (solidColor) {
        colors = std::vector<Color> {
                color,
                color,
                color,
        };
    } else {
        colors = std::vector<Color> {
                Color(1, 0, 0),
                Color(0, 1, 0),
                Color(0, 0, 1),
        };
    }

    int colorIdx = 0;

    int top = vertexes.size() - 2;
    if (top == 0) return frags;

    for (int i = 0; i <= top; ++i) {
        const auto& p2 = vertexes[i];
        const auto& p3 = vertexes[i + 1];
        for (float t = 0; t < 1; t += .001f) {
            Vector4 p_t;
            auto selectedColor = (1 - t) * colors[colorIdx % 3] + t * colors[(colorIdx + 1) % 3];
            if (i == 0) {
                p_t = Pt(t - 1, p2, p3, vertexes[i + 2]);
            } else if (i == top) {
                p_t = Pt(t, vertexes[i - 1], p2, p3);
            } else {
                p_t = (1 - t) * Pt(t, vertexes[i - 1], p2, p3) + t * Pt(t - 1, p2, p3, vertexes[i + 2]);
            }
            auto point = (transform * p_t).toHomogenous();
            frags.push_back({static_cast<int>(roundf(point.x)), static_cast<int>(roundf(point.y)), point.z, p_t, {}, selectedColor});
        }
        colorIdx++;
    }

    return frags;
}
