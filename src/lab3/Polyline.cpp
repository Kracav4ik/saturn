#include "Polyline.h"

#include "OpenLL/Vector4.h"
#include "OpenLL/DrawAPI.h"

const int NO_SELECTION = -1;
const float RADIUS = 0.1;
const int PIXEL_RADIUS = 10;

using namespace ll;

Polyline::Polyline(const Vector4& vertex, Color color)
    : SAD(vertex, color)
{
}

Polyline::Polyline()
    : SAD()
{
}

void Polyline::addVertex(const Vector4& pos) {
    if (isLastSelected()) {
        vertexes.push_back(pos);
        currentSelection++;
    }
    if (isFirstSelected()) {
        vertexes.insert(vertexes.begin(), pos);
    }
}

bool Polyline::onBorder() const {
    return isFirstSelected() || isLastSelected();
}

void Polyline::setPos(const Vector4& pos) {
    if (isValid()) {
        SAD::setPos(pos);
    }
}

bool Polyline::isDrawSelection() const {
    return SAD::isDrawSelection() && isValid();
}

void Polyline::draw(DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const {
    std::vector<Line> lines;
    for (int i = 1; i < vertexes.size(); ++i) {
        Vertex from {color, vertexes[i-1]};
        Vertex to {color, vertexes[i]};
        lines.emplace_back(from, to);
    }
    if (drawLines) {
        drawAPi.addShapes(lines);
    }
}

bool Polyline::isValid() const {
    return currentSelection > -1 && currentSelection < vertexes.size();
}

bool Polyline::isFirstSelected() const {
    return currentSelection == 0;
}

bool Polyline::isLastSelected() const {
    return currentSelection == vertexes.size() - 1;
}

void Polyline::setDrawLines(bool isDraw) {
    drawLines = isDraw;
}
