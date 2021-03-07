#pragma once

#include "Vertex.h"
#include "Shape.h"

namespace ll {

struct Line : Shape {
    Line(Vertex points0, Vertex points1);

    std::vector<Fragment> getFragments(Framebuffer& fb, const Matrix4x4& transform, CullMode cull) const override;

    Vertex points[2];
};

}
