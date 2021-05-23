#include "BezierSurface.h"

#include "Fragment.h"
#include "Vertex.h"
#include "Matrix4x4.h"
#include "DrawAPI.h"

#include <omp.h>

using namespace ll;

template <auto Vertex::* m>
auto bern(float u, float v, const std::vector<std::vector<Vertex>>& vertexes) {
    float mu = 1 - u;
    float u_mul[] {
            1 * mu * mu * mu,
            3 * u * mu * mu,
            3 * u * u * mu,
            1 * u * u * u,
    };

    float mv = 1 - v;
    float v_mul[] {
            1 * mv * mv * mv,
            3 * v * mv * mv,
            3 * v * v * mv,
            1 * v * v * v,
    };
    return  u_mul[0] * v_mul[0] * vertexes[0][0].*m +
            u_mul[0] * v_mul[1] * vertexes[0][1].*m +
            u_mul[0] * v_mul[2] * vertexes[0][2].*m +
            u_mul[0] * v_mul[3] * vertexes[0][3].*m +

            u_mul[1] * v_mul[0] * vertexes[1][0].*m +
            u_mul[1] * v_mul[1] * vertexes[1][1].*m +
            u_mul[1] * v_mul[2] * vertexes[1][2].*m +
            u_mul[1] * v_mul[3] * vertexes[1][3].*m +

            u_mul[2] * v_mul[0] * vertexes[2][0].*m +
            u_mul[2] * v_mul[1] * vertexes[2][1].*m +
            u_mul[2] * v_mul[2] * vertexes[2][2].*m +
            u_mul[2] * v_mul[3] * vertexes[2][3].*m +

            u_mul[3] * v_mul[0] * vertexes[3][0].*m +
            u_mul[3] * v_mul[1] * vertexes[3][1].*m +
            u_mul[3] * v_mul[2] * vertexes[3][2].*m +
            u_mul[3] * v_mul[3] * vertexes[3][3].*m;
}


Vertex getPoint(float u, float v, const std::vector<std::vector<Vertex>>& vertexes) {
    return {
        vertexes[0][0].color,
        bern<&Vertex::pos>(u, v, vertexes),
        bern<&Vertex::uv>(u, v, vertexes),
    };
}

BezierSurface::BezierSurface(std::vector<std::vector<Vertex>> vertexes)
        : vertexes(std::move(vertexes)) {
}

std::vector<Fragment> BezierSurface::getFragments(Framebuffer& fb, const Matrices& matrices, CullMode cull) const {
    //*
    std::vector<Fragment> frags;
    const int N = 500;
    const float step = 1.0f/N;
    frags.resize((N+1)*(N+1));

#pragma omp parallel for default(none) shared(matrices, frags)
    for (int uI = 0; uI <= N; ++uI) {
        float u = uI*step;
        for (int vI = 0; vI <= N; ++vI) {
            float v = vI*step;
            auto vertex = getPoint(u, v, vertexes);
            auto point = (matrices.fullTransform * vertex.pos).toHomogenous();
            auto world = (matrices.model * vertex.pos).toHomogenous();
            frags[vI + uI*(N+1)].color = vertex.color;
            frags[vI + uI*(N+1)].uv = vertex.uv;
            frags[vI + uI*(N+1)].x = static_cast<int>(roundf(point.x));
            frags[vI + uI*(N+1)].y = static_cast<int>(roundf(point.y));
            frags[vI + uI*(N+1)].z = point.z;
            frags[vI + uI*(N+1)].world = world;
        }
    }
/*/
    auto frags = std::vector<Fragment>();
    for (float u = 0; u <= 1; u += 0.002f) {
        for (float v = 0; v <= 1; v += 0.002f) {
            const Vertex& vertex = getPoint(u, v, vertexes);
            auto point = (transform * vertex.pos).toHomogenous();
            frags.push_back({
                    static_cast<int>(roundf(point.x)),
                    static_cast<int>(roundf(point.y)),
                    point.z, vertex.uv, vertex.color
            });
        }
    }
    //*/

    return frags;
}
