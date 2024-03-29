#pragma once

#include "Shape.h"
#include "Vertex.h"

namespace ll {

struct Triangle : Shape {
    Triangle(Vertex point0, Vertex point1, Vertex point2);

    Vertex points[3];

    std::vector<Fragment> getFragments(ll::Framebuffer& fb, const Matrices& matrices, CullMode cull) const override;
};

}
