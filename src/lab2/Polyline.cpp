#include "Polyline.h"

#include "OpenLL/DrawAPI.h"

const int NO_SELECTION = -1;
const float RADIUS = 0.1;

using namespace ll;

Polyline::Polyline(const Vector4& vertex, Color color)
    : vertexes(1, vertex)
    , drawSelection(false)
    , drawPreselection(false)
    , currentPreselection(NO_SELECTION)
    , currentSelection(0)
    , color(color)
{
}

Polyline::Polyline()
    : vertexes()
    , currentPreselection(NO_SELECTION)
    , currentSelection(NO_SELECTION)
    , color(Color::DISCARD)
{
}

void Polyline::addVertex(const Vector4& pos) {
    if (isFirstSelected()) {
        vertexes.insert(vertexes.begin(), pos);
    }
    if (isLastSelected()) {
        vertexes.push_back(pos);
        currentSelection++;
    }
}

void Polyline::removeVertex() {
    if (isValid()) {
        vertexes.erase(vertexes.begin() + currentSelection);
    }
}

bool Polyline::onBorder() const {
    return isFirstSelected() || isLastSelected();
}

void Polyline::setPos(const Vector4& pos) {
    if (isValid()) {
        vertexes[currentSelection] = pos;
    }
}

void Polyline::changeCurrentSelection(int newSelection) {
    currentSelection = newSelection;
}

void Polyline::changeColor(Color newColor) {
    color = newColor;
}

int Polyline::getCurrentSelection() const {
    return currentSelection;
}

int Polyline::getCurrentPreselection() const {
    return currentPreselection;
}

void Polyline::draw(DrawAPI& drawAPi) const {
    std::vector<Line> lines;
    for (int i = 1; i < vertexes.size(); ++i) {
        Vertex from {color, vertexes[i-1]};
        Vertex to {color, vertexes[i]};
        lines.emplace_back(from, to);
    }
    drawAPi.addLines(lines);
    if (drawSelection) {
        if (isValid()) {
            drawAPi.drawRound({color, vertexes[currentSelection]}, RADIUS, true);
        }
    }

    if (drawPreselection) {
        if (currentPreselection != NO_SELECTION) {
            drawAPi.drawRound({color, vertexes[currentPreselection]}, RADIUS, false);
        }
    }
}

std::vector<Vector4> Polyline::getVertexes() const {
    return vertexes;
}

ll::Vector4 Polyline::getSelVertex() const {
    if (currentSelection != NO_SELECTION) {
        return vertexes[currentSelection];
    }
    return {};
}

void Polyline::select() {
    if (currentPreselection != NO_SELECTION) {
        currentSelection = currentPreselection;
        currentPreselection = NO_SELECTION;
    } else {
        currentSelection = NO_SELECTION;
    }
}

void Polyline::createNew(const Vector4& pos) {
    if (currentSelection != NO_SELECTION) {
        addVertex(pos);
    }
}

void Polyline::preselect(int idx) {
    currentPreselection = idx;
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

void Polyline::setDrawSelection(bool drawSel) {
    drawSelection = drawSel;
}

void Polyline::setDrawPreselection(bool drawPresel) {
    drawPreselection = drawPresel;
}
