#include "SelectedAndDraggable.h"

#include "OpenLL/Vector4.h"
#include "OpenLL/DrawAPI.h"

using namespace ll;

SelectedAndDraggable::SelectedAndDraggable(const ll::Vector4& vertex, ll::Color color)
    : vertexes(1, vertex)
    , drawSelection(false)
    , drawPreselection(false)
    , currentPreselection(NO_SELECTION)
    , currentSelection(0)
    , color(color)
{
}

SelectedAndDraggable::SelectedAndDraggable()
        : vertexes()
        , currentPreselection(NO_SELECTION)
        , currentSelection(NO_SELECTION)
        , color(Color::DISCARD)
{
}

void SelectedAndDraggable::addVertex(const ll::Vector4& pos) {
    vertexes.push_back(pos);
}

void SelectedAndDraggable::removeVertex() {
    if(currentPreselection != NO_SELECTION) {
        if (currentSelection == currentPreselection) {
            currentSelection = NO_SELECTION;
        }
        vertexes.erase(vertexes.begin() + currentPreselection);
        currentPreselection = NO_SELECTION;
    }
}

void SelectedAndDraggable::changeCurrentSelection(int newSelection) {
    currentSelection = newSelection;
}

void SelectedAndDraggable::changeColor(ll::Color newColor) {
    color = newColor;
}

void SelectedAndDraggable::draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const {
    drawSelPlesel(drawAPi, viewProjection);
}

void SelectedAndDraggable::setPos(const ll::Vector4& pos) {
    vertexes[currentSelection] = pos;
}

int SelectedAndDraggable::getCurrentSelection() const {
    return currentSelection;
}

int SelectedAndDraggable::getCurrentPreselection() const {
    return currentPreselection;
}

bool SelectedAndDraggable::isDrawSelection() const {
    return drawSelection;
}

const std::vector<ll::Vector4>& SelectedAndDraggable::getVertexes() const {
    return vertexes;
}

ll::Vector4 SelectedAndDraggable::getSelVertex() const {
    if (currentSelection != NO_SELECTION) {
        return vertexes[currentSelection];
    }
    return {};
}

void SelectedAndDraggable::setDrawSelection(bool drawSel) {
    drawSelection = drawSel;
}

void SelectedAndDraggable::setDrawPreselection(bool drawPresel) {
    drawPreselection = drawPresel;
}

const ll::Color& SelectedAndDraggable::getColor() const {
    return color;
}

void SelectedAndDraggable::drawSelPlesel(ll::DrawAPI& drawAPi, const ll::Matrix4x4& viewProjection) const {

    for (const auto& vertex : vertexes) {
        drawAPi.drawSphere({Color(1, 0, 0), vertex}, 0.04);
    }
    if (currentSelection == NO_SELECTION) return;
    {
        auto wrapper = drawAPi.saveTransform();
        drawAPi.loadIdentity();
        if (drawSelection) {
            if (!vertexes.empty()) {
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

void SelectedAndDraggable::select() {
    if (currentPreselection != NO_SELECTION) {
        currentSelection = currentPreselection;
        currentPreselection = NO_SELECTION;
    } else {
        currentSelection = NO_SELECTION;
    }
}

void SelectedAndDraggable::createNew(const Vector4& pos) {
    if (currentSelection != NO_SELECTION && drawSelection) {
        addVertex(pos);
    }
}

void SelectedAndDraggable::preselect(int idx) {
    currentPreselection = idx;
}
