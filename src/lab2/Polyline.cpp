#include "Polyline.h"

#include "OpenLL/DrawAPI.h"

const int NO_SELECTION = -1;
const float RADIUS = 0.1;
const int PIXEL_RADIUS = 10;

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
    if (isLastSelected()) {
        vertexes.push_back(pos);
        currentSelection++;
    }
    if (isFirstSelected()) {
        vertexes.insert(vertexes.begin(), pos);
    }
}

void Polyline::removeVertex() {
    if(currentPreselection != NO_SELECTION) {
        if (currentSelection == currentPreselection) {
            currentSelection = NO_SELECTION;
        }
        vertexes.erase(vertexes.begin() + currentPreselection);
        currentPreselection = NO_SELECTION;
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

bool Polyline::isDrawSelection() const {
    return drawSelection && isValid();
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
    {
        auto wrapper = drawAPi.saveTransform();
        drawAPi.loadIdentity();
        if (drawSelection) {
            if (isValid()) {
                drawAPi.drawRound({color, viewProjection * vertexes[currentSelection]}, PIXEL_RADIUS, true);
            }
        }

        if (drawPreselection) {
            if (currentPreselection != NO_SELECTION) {
                drawAPi.drawRound({color, viewProjection * vertexes[currentPreselection]}, PIXEL_RADIUS, false);
            }
        }
    }
}

const std::vector<ll::Vector4>& Polyline::getVertexes() const {
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
    if (currentSelection != NO_SELECTION && drawSelection) {
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

void Polyline::setDrawLines(bool isDraw) {
    drawLines = isDraw;
}
