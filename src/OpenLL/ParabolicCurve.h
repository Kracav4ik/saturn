#pragma once

#include "Shape.h"
#include "Vector4.h"
#include "Color.h"

namespace ll {

struct ParabolicCurve : Shape {
    explicit ParabolicCurve(std::vector<Vector4> vertexes);

    std::vector<Fragment> getFragments(ll::Framebuffer& fb, const ll::Matrix4x4& transform, CullMode cull) const override;

    std::vector<Vector4> vertexes;
};

}
