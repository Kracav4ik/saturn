#include "Model.h"
#include "OpenLL/DrawAPI.h"
#include "OpenLL/ParabolicCurve.h"

void Model::draw(ll::DrawAPI& drawApi, ll::Matrix4x4 viewProjection) {
    if (isShiftPressed) {
        auto mouseColor = ll::Color(0, 1, 0);
        ll::Vertex mouseVert = {mouseColor, currentMouseWorldPos};
        drawApi.drawLinesCube(
                currentMouseWorldPos,
                2 * RADIUS,
                mouseColor
        );

        if (lastSel != NO_SELECTION && polylines[lastSel]->onBorder()) {
            drawApi.addShapes<ll::Line>({ll::Line({mouseColor, polylines[lastSel]->getSelVertex()}, mouseVert)});
        }
    }

    for (const auto& polyline : polylines) {
        polyline->draw(drawApi, viewProjection);
        drawApi.addShapes<ll::ParabolicCurve>({ll::ParabolicCurve(polyline->getVertexes())});
    }
}

bool Model::getLastSelVis() const {
    if (lastSel != NO_SELECTION) {
        return polylines[lastSel]->isDrawSelection();
    }
    return false;
}

ll::Vector4 Model::getSelPoint() const {
    if (lastSel != NO_SELECTION) {
        return polylines[lastSel]->getSelVertex();
    }
    return {};
}

void Model::setSelPoint(const ll::Vector4& vec) {
    if (lastSel != NO_SELECTION) {
        polylines[lastSel]->setPos(vec);
    }
}

void Model::setDrawLines(bool isDraw) {
    for (const auto& polyline : polylines) {
        polyline->setDrawLines(isDraw);
    }
}

void Model::press(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 viewProjection) {
    isShiftPressed = withShift;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 240 - currentMouseCameraPos.y, 1);
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    dragging = true;

    if (isShiftPressed) {
        if (!polylines.empty() && polylines.back()->onBorder()) {
            emit clickWithShift(currentMouseWorldPos);
            return;
        }
        polylines.push_back(std::make_unique<Polyline>(currentMouseWorldPos, ll::Color(0, 0, 1)));
        selectPolyline(polylines.size() - 1, false);
        lastSel = polylines.size() - 1;
        connect(this, &Model::clickWithShift, polylines.back().get(), &Polyline::createNew);
        connect(this, &Model::click, polylines.back().get(), &Polyline::select);
        connect(this, &Model::presel, polylines.back().get(), &Polyline::preselect);
    } else {
        emit click();
        selectPolyline(lastPresel, false);
        lastSel = lastPresel;
    }
}

void Model::pressRight() {
    if (lastPresel != NO_SELECTION) {
        polylines[lastPresel]->removeVertex();
        if (polylines[lastPresel]->getVertexes().size() == 0) {
            polylines.erase(polylines.begin() + lastPresel);
            lastSel = NO_SELECTION;
            lastPresel = NO_SELECTION;
        }
    }
}

void Model::move(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 viewProjection) {
    isShiftPressed = withShift;
    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 240 - currentMouseCameraPos.y, 1);

    if (dragging && isInside) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        auto pos = polylines[lastSel]->getSelVertex();
        auto newPos = pos + viewProjection.inverse() * ll::Vector4::direction(diff.x, -diff.y, 0);
        polylines[lastSel]->setPos(newPos.toHomogenous());
    }

    if (isShiftPressed) {
        return;
    }

    emit presel(NO_SELECTION);

    float closestPoint = PIXEL_RADIUS;
    int closestPointIdx = -1;
    int closestPolylineIdx = -1;

    for (int i = 0; i < polylines.size(); ++i) {
        auto curVerts = polylines[i]->getVertexes();
        for (int j = 0; j < curVerts.size(); ++j) {
            auto vector4 = viewProjection * curVerts[j];
            vector4.z = 0;
            float newPoint = (vector4 - currentTranslatedMouseCameraPos).normal();
            if (newPoint < closestPoint) {
                closestPoint = newPoint;
                closestPointIdx = j;
                closestPolylineIdx = i;
            }
        }
    }

    if (closestPointIdx != -1) {
        isInside = true;
        selectPolyline(closestPolylineIdx, true);
        lastPresel = closestPolylineIdx;
        emit presel(closestPointIdx);
    } else {
        isInside = false;
    }
}

void Model::release() {
    dragging = false;
}

void Model::selectPolyline(int idx, bool presel) {
    for (int i = 0; i < polylines.size(); ++i) {
        if (i != idx) {
            if (presel) {
                polylines[i]->setDrawPreselection(false);
            } else {
                polylines[i]->setDrawSelection(false);
            }
        } else {
            if (presel) {
                polylines[i]->setDrawPreselection(true);
            } else {
                polylines[i]->setDrawSelection(true);
            }
        }
    }
}
