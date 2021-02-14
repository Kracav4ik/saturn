#include "DrawAPI.h"
#include "Framebuffer.h"

#include <cmath>

using namespace ll;

namespace {

struct FragmentLine {
    int x0;
    int x1;
    int y;
};

bool edge(const Vector4& p, const Vector4& v1, const Vector4& v2) {
    return (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x) >= 0;
}

void collectFrags(int w, int h, std::vector<FragmentLine>& frags, const Triangle& triangle) {
    const auto& v = triangle.points;
    Vector4 leftTop{static_cast<float>(w), static_cast<float>(h), 0, 0};
    Vector4 rightBottom{0, 0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        leftTop.x = std::min(leftTop.x, v[i].x);
        leftTop.y = std::min(leftTop.y, v[i].y);
        rightBottom.x = std::max(rightBottom.x, v[i].x);
        rightBottom.y = std::max(rightBottom.y, v[i].y);
    }
    int bbMinX = static_cast<int>(floorf(leftTop.x));
    int bbMinY = static_cast<int>(floorf(leftTop.y));
    int bbMaxX = static_cast<int>(ceilf(rightBottom.x));
    int bbMaxY = static_cast<int>(ceilf(rightBottom.y));

    for (int y = bbMinY; y <= bbMaxY; ++y) {
        int xFragStart;
        int xFragEnd;
        bool fragStarted = false;
        // TODO: calculate xFragStart and xFragEnd instead of iterating over whole line
        // TODO: handle common edge for triangles
        for (int x = bbMinX; x <= bbMaxX; ++x) {
            Vector4 p{static_cast<float>(x), static_cast<float>(y), 0, 0};
            bool isInside = edge(p, v[0], v[1]) && edge(p, v[1], v[2]) && edge(p, v[2], v[0]);
            if (isInside) {
                if (!fragStarted) {
                    xFragStart = x;
                    fragStarted = true;
                }
                xFragEnd = x;
            } else {
                if (fragStarted) {
                    break;
                }
            }
        }
        if (fragStarted) {
            frags.push_back(FragmentLine{xFragStart, xFragEnd, y});
        }
    }
}

}

void DrawAPI::setFragmentShader(Shader shader) {
    fragmentShader = std::move(shader);
}

void DrawAPI::reset() {
    objects.clear();
}

void DrawAPI::clear(Framebuffer& fb, const Color& color) const {
    std::fill(fb.colors.begin(), fb.colors.end(), color);
}

void DrawAPI::addTriangles(const std::vector<Triangle>& triangles) {
    objects.insert(objects.end(), triangles.begin(), triangles.end());
}

void DrawAPI::drawFrame(Framebuffer& fb) const {
    std::vector<FragmentLine> fragments;

    for (const auto& object : objects) {
        collectFrags(fb.getW(), fb.getH(), fragments, object);
    }

    for (const auto& frag : fragments) {
        for (int x = frag.x0; x <= frag.x1; ++x) {
            fb.at(x, frag.y) = fragmentShader(x, frag.y);
        }
    }
}
