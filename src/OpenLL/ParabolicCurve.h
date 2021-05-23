#pragma once

#include "Shape.h"
#include "Color.h"

namespace ll {

struct Vector4;

struct ParabolicCurve : Shape {
    explicit ParabolicCurve(std::vector<Vector4> vertexes, bool solidColor=false, Color color=Color(0,0,0));

    std::vector<Fragment> getFragments(ll::Framebuffer& fb, const Matrices& matrices, CullMode cull) const override;

    std::vector<Vector4> vertexes;
    bool solidColor;
    Color color;
};

}
