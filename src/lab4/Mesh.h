#pragma once

#include "OpenLL/Vector4.h"
#include "OpenLL/Matrix4x4.h"
#include "OpenLL/Color.h"
#include "SelectedAndDraggable.h"

#include <vector>
#include <QObject>
#include <memory>

namespace ll {
class DrawAPI;
class Vector4;
}

struct TripletsVector4;

class Mesh : public SAD {
public:
    Mesh(const ll::Vector4& vertex, const ll::Color& color);
    Mesh();

    void addVertex(const ll::Vector4& pos) override;
    void draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const override;
    std::pair<int, int> get2Mins(const ll::Vector4& pos) const;
    void setDrawTriangles(bool drawTriangles);

    ~Mesh();

private:
    std::vector<std::unique_ptr<TripletsVector4>> tripletsVexes;
};
