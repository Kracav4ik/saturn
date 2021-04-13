#include "Mesh.h"
#include "OpenLL/DrawAPI.h"

using namespace ll;

struct TripletsVector4 {
    TripletsVector4(int p1, int p2, int p3, bool drawTriangles=false)
    : p1(p1)
    , p2(p2)
    , p3(p3)
    , drawTriangles(drawTriangles)
{
}

    int p1;
    int p2;
    int p3;
    bool drawTriangles;

    void draw(DrawAPI& drawAPi, Color color, const std::vector<ll::Vector4>& vec) const {
        if (drawTriangles) {
            drawAPi.addShapes<Triangle>({{{color, vec[p1]}, {color, vec[p2]}, {color, vec[p3]}}});
        } else {
            drawAPi.addShapes<Line>({
                {{color, vec[p1], {}}, {color, vec[p2], {}}},
                {{color, vec[p1], {}}, {color, vec[p3], {}}},
                {{color, vec[p2], {}}, {color, vec[p3], {}}},
            });
        }
    }
};

void Mesh::draw(DrawAPI& drawAPi, Matrix4x4 viewProjection) const {
    SAD::draw(drawAPi, viewProjection);
    if (tripletsVexes.empty()) {
        drawAPi.addShapes<Line>({{{color, vertexes[0]}, {color, vertexes[1]}}});
    }

    for (const auto& tripVex : tripletsVexes) {
        tripVex->draw(drawAPi, getColor(), getVertexes());
    }
}

Mesh::Mesh(const Vector4& vertex, const Color& color)
    : SAD(vertex, color)
{
}

Mesh::Mesh() {}

void Mesh::addVertex(const Vector4& pos) {
    auto mins = get2Mins(pos);

    SAD::addVertex(pos);
    currentSelection = vertexes.size() - 1;

    const auto& vertexes = getVertexes();

    if (vertexes.size() < 3) return;

    bool drawTriangles = false;
    if (!tripletsVexes.empty()) {
        drawTriangles = tripletsVexes.back()->drawTriangles;
    }
    
    tripletsVexes.push_back(std::make_unique<TripletsVector4>(mins.first, mins.second, vertexes.size() - 1, drawTriangles));
}

std::pair<int, int> Mesh::get2Mins(const Vector4& pos) const {
    int min1 = 0;
    int min2 = 1;
    auto minDist1 = (vertexes[min1] - pos).normal();
    auto minDist2 = (vertexes[min2] - pos).normal();
    if (minDist1 > minDist2) {
        std::swap(min1, min2);
        std::swap(minDist1, minDist2);
    }

    for (int i = 2; i < vertexes.size(); ++i) {
        auto curDist = (vertexes[i] - pos).normal();
        if (minDist1 > curDist) {
            min2 = min1;
            minDist2 = minDist1;
            min1 = i;
            minDist1 = curDist;
        } else if (minDist2 > curDist) {
            min2 = i;
            minDist2 = curDist;
        }
    }
    return {min1, min2};
}

void Mesh::setDrawTriangles(bool drawTriangles) {
    for (const auto& tripletsVex : tripletsVexes) {
        tripletsVex->drawTriangles = drawTriangles;
    }
}

Mesh::~Mesh() = default;
