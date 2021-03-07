#include "Line.h"

#include "Fragment.h"
#include "Framebuffer.h"
#include "Matrix4x4.h"

using namespace ll;

Line::Line(Vertex points0, Vertex points1)
    : points{points0, points1}
{
}


std::vector<Fragment> Line::getFragments(Framebuffer& fb, const Matrix4x4& transform, CullMode cull) const {
    std::vector<Fragment> frags;

    Color c[2] = {
            points[0].color,
            points[1].color,
    };

    Vector4 v[2] = {
            transform * points[0].pos,
            transform * points[1].pos,
    };

    auto makeUV4D = [&](int i) {
        const auto& uv = points[i].uv;
        float w = v[i].w;
        if (w != 0) {
            return Vector4{uv.u/w, uv.v/w, 0, 1/w};
        }
        return Vector4{uv.u, uv.v, 0, 0};
    };

    Vector4 uv[2] = {
            makeUV4D(0),
            makeUV4D(1),
    };

    for (auto& pos : v) {
        pos = pos.toHomogenous();
        pos.x = roundf(pos.x);
        pos.y = roundf(pos.y);
    }

    auto diff = (v[1] - v[0]);

    float width = std::abs(diff.x);
    float height = std::abs(diff.y);

    int n = std::max(width, height);

    for (int i = 0; i <= n ; ++i) {
        float t = static_cast<float>(i)/static_cast<float>(n);
        Vector4 p = v[0] + t * diff;

        Vector4 fragUV = ((1 - t) * uv[0] + t * uv[1]).toHomogenous();
        Color color = (1 - t) * c[0] + t * c[1];

        frags.push_back({static_cast<int>(roundf(p.x)), static_cast<int>(roundf(p.y)), p.z, {fragUV.x, fragUV.y}, color});
    }

    return frags;
}
