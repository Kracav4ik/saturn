#include "Triangle.h"

#include "Sampler.h"
#include "Framebuffer.h"
#include "Fragment.h"
#include "DrawAPI.h"

#include <cmath>

using namespace ll;

namespace {

float edge(const Vector4& p, const Vector4& v1, const Vector4& v2) {
    return (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x);
}

}

Triangle::Triangle(Vertex point0, Vertex point1, Vertex point2)
    : points{point0, point1, point2}
{
}

std::vector<Fragment> Triangle::getFragments(ll::Framebuffer& fb, const ll::Matrices& matrices, CullMode cull) const {
    std::vector<Fragment> frags;

    Color c[3] = {
            points[0].color,
            points[1].color,
            points[2].color,
    };

    Vector4 v[3] = {
            matrices.fullTransform * points[0].pos,
            matrices.fullTransform * points[1].pos,
            matrices.fullTransform * points[2].pos,
    };
    Vector4 world[3] = {
            matrices.model * points[0].pos,
            matrices.model * points[1].pos,
            matrices.model * points[2].pos,
    };

    auto makeUV4D = [&](int i) {
        const auto& uv = points[i].uv;
        float w = v[i].w;
        if (w != 0) {
            return Vector4{uv.u/w, uv.v/w, 0, 1/w};
        }
        return Vector4{uv.u, uv.v, 0, 0};
    };

    Vector4 uv[3] = {
            makeUV4D(0),
            makeUV4D(1),
            makeUV4D(2),
    };

    for (auto& pos : v) {
        pos = pos.toHomogenous();
    }

    Vector4 leftTop{static_cast<float>(fb.getW()), static_cast<float>(fb.getH()), 0, 0};
    Vector4 rightBottom{0, 0, 0, 0};
    for (const auto& pos : v) {
        leftTop.x = std::min(leftTop.x, pos.x);
        leftTop.y = std::min(leftTop.y, pos.y);
        rightBottom.x = std::max(rightBottom.x, pos.x);
        rightBottom.y = std::max(rightBottom.y, pos.y);
    }
    int bbMinX = std::max(0, static_cast<int>(floorf(leftTop.x)));
    int bbMinY = std::max(0, static_cast<int>(floorf(leftTop.y)));
    int bbMaxX = std::min(fb.getW() - 1, static_cast<int>(ceilf(rightBottom.x)));
    int bbMaxY = std::min(fb.getH() - 1, static_cast<int>(ceilf(rightBottom.y)));

    float area = edge(v[0], v[1], v[2]);
    if (area == 0) {
        return {};
    }

    for (int y = bbMinY; y <= bbMaxY; ++y) {
        bool fragStarted = false;
        // TODO: handle common edge for triangles
        for (int x = bbMinX; x <= bbMaxX; ++x) {
            Vector4 p{static_cast<float>(x), static_cast<float>(y), 0, 0};
            float w2 = edge(p, v[0], v[1]);
            float w0 = edge(p, v[1], v[2]);
            float w1 = edge(p, v[2], v[0]);
            bool isInsideCW = w0 >= 0 && w1 >= 0 && w2 >= 0;
            bool isInsideCCW = w0 <= 0 && w1 <= 0 && w2 <= 0;
            bool isInside = (isInsideCW && cull != CullMode::DrawCCW) || (isInsideCCW && cull != CullMode::DrawCW);
            if (isInside) {
                if (!fragStarted) {
                    fragStarted = true;
                }
                w0 /= area;
                w1 /= area;
                w2 = 1 - w0 - w1;
                Vector4 worldPos = w0 * world[0] + w1 * world[1] + w2 * world[2];
                Vector4 fragUV = (w0 * uv[0] + w1 * uv[1] + w2 * uv[2]).toHomogenous();
                Vertex vert{
                        w0 * c[0] + w1 * c[1] + w2 * c[2],
                        w0 * v[0] + w1 * v[1] + w2 * v[2],
                        {fragUV.x, fragUV.y},
                };
                frags.push_back({x, y, vert.pos.z, worldPos, vert.uv, vert.color});
            } else {
                if (fragStarted) {
                    break;
                }
            }
        }
    }

    return frags;
}
