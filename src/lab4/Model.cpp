#include "Model.h"
#include "OpenLL/DrawAPI.h"

#include "LinesWithClipRect.h"

Model::Model(QObject* parent) : QObject(parent) {

    sads.push_back(std::make_unique<LinesWithClipRect>(ll::Vector4::position(1, 1, 0 ), 0.4, 0.4));
    lastSel = 0;

    sads.back()->changeColor(ll::Color(0, 1, 0));

    connect(this, &Model::click, sads.back().get(), &LinesWithClipRect::select);
    connect(this, &Model::presel, sads.back().get(), &LinesWithClipRect::preselect);
}

void Model::draw(ll::DrawAPI& drawApi, ll::Matrix4x4 viewProjection) {
    if ((keyMods & Qt::ShiftModifier) != 0) {
        auto color = ll::Color(1, 0, 0);
        drawApi.drawLinesCube(
                currentMouseWorldPos,
                2 * RADIUS,
                color
        );
    }
    for (const auto& sad : sads) {
        sad->draw(drawApi, viewProjection);
    }
}

bool Model::getLastSelVis() const {
    if (lastSel != NO_SELECTION) {
        return sads[lastSel]->isDrawSelection();
    }
    return false;
}

ll::Vector4 Model::getSelPoint() const {
    if (lastSel != NO_SELECTION) {
        return sads[lastSel]->getSelVertex();
    }
    return {};
}

std::vector<ll::Vector4> Model::getSelVexes() const {
    if (lastSel != NO_SELECTION) {
        return sads[lastSel]->getVertexes();
    }
    return {};
}

void Model::setSelPoint(const ll::Vector4& vec) {
    if (lastSel != NO_SELECTION) {
        sads[lastSel]->setPos(vec);
    }
}

void Model::setDrawLines(bool isDraw) {
//    for (const auto& mesh : sads) {
//        mesh->setDrawLines(isDraw);
//    }
}

void Model::setDrawTriangles(bool drawTriangles) {
//    for (const auto& mesh : sads) {
//        mesh->setDrawTriangles(drawTriangles);
//    }
}

void Model::press(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection) {
    keyMods = modifiers;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 480 - currentMouseCameraPos.y, 1);
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    dragging = true;

    if ((modifiers & Qt::ShiftModifier) != 0) {
        sads.back()->addVertex(currentMouseWorldPos);
        return;
    }

    if (lastPresel != NO_SELECTION) {
        emit click();
        selectSAD(lastPresel, false);
    }
}

void Model::pressRight() {
//    if (lastPresel != NO_SELECTION) {
//        sads[lastPresel]->removeVertex();
//        if (sads[lastPresel]->getVertexes().size() == 0) {
//            sads.erase(sads.begin() + lastPresel);
//            lastSel = NO_SELECTION;
//            lastPresel = NO_SELECTION;
//        }
//    }
}

void Model::move(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection) {
    keyMods = modifiers;
    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 480 - currentMouseCameraPos.y, 1);

    if (dragging && isInside) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        auto pos = sads[lastSel]->getSelVertex();
        auto newPos = pos + viewProjection.inverse() * ll::Vector4::direction(diff.x, -diff.y, 0);
        sads[lastSel]->setPos(newPos.toHomogenous());
    }

    if ((keyMods & (Qt::ShiftModifier | Qt::ControlModifier)) != 0) {
        return;
    }

    emit presel(NO_SELECTION);

    float closestPoint = PIXEL_RADIUS;
    int closestPointIdx = -1;
    int closestSADIdx = -1;

    for (int i = 0; i < sads.size(); ++i) {
        auto curVerts = sads[i]->getVertexes();
        for (int j = 0; j < curVerts.size(); ++j) {
            auto vector4 = viewProjection * curVerts[j];
            vector4.z = 0;
            float newPoint = (vector4 - currentTranslatedMouseCameraPos).normal();
            if (newPoint < closestPoint) {
                closestPoint = newPoint;
                closestPointIdx = j;
                closestSADIdx = i;
            }
        }
    }

    if (closestPointIdx != -1) {
        isInside = true;
        selectSAD(closestSADIdx, true);
        lastPresel = closestSADIdx;
        emit presel(closestPointIdx);
    } else {
        isInside = false;
    }
}

void Model::release() {
    dragging = false;
}

void Model::selectSAD(int idx, bool presel) {
    for (int i = 0; i < sads.size(); ++i) {
        if (i != idx) {
            if (presel) {
                sads[i]->setDrawPreselection(false);
            } else {
                sads[i]->setDrawSelection(false);
            }
        } else {
            if (presel) {
                sads[i]->setDrawPreselection(true);
            } else {
                sads[i]->setDrawSelection(true);
            }
        }
    }
}
