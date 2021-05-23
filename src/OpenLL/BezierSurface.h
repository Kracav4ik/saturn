#pragma once

#include "Shape.h"
#include "Color.h"

namespace ll {

struct Vertex;

struct BezierSurface : Shape {
    explicit BezierSurface(std::vector<std::vector<Vertex>> vertexes);

    std::vector<Fragment> getFragments(ll::Framebuffer& fb, const ll::Matrices& matrices, CullMode cull) const override;

    std::vector<std::vector<Vertex>> vertexes;
};

}
