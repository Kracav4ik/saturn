#include "Model.h"
#include "OpenLL/DrawAPI.h"
#include "Mesh.h"

void Model::draw(ll::DrawAPI& drawApi, ll::Matrix4x4 viewProjection) {
    if ((keyMods & Qt::ShiftModifier) != 0) {
        auto color = ll::Color(1, 0, 0);
        drawApi.drawLinesCube(
                currentMouseWorldPos,
                2 * RADIUS,
                color
        );
        if (lastSel != NO_SELECTION) {
            if (meshes[lastSel]->getVertexes().size() > 1) {
                auto mins = meshes[lastSel]->get2Mins(currentMouseWorldPos);
                drawApi.addLines({ll::Line({color, meshes[lastSel]->getVertexes()[mins.first]}, {color, currentMouseWorldPos})});
                drawApi.addLines({ll::Line({color, meshes[lastSel]->getVertexes()[mins.second]}, {color, currentMouseWorldPos})});
            } else {
                drawApi.addLines({ll::Line({color, meshes[lastSel]->getSelVertex()}, {color, currentMouseWorldPos})});
            }
        }
    }
    for (const auto& mesh : meshes) {
        mesh->draw(drawApi, viewProjection);
    }
}

bool Model::getLastSelVis() const {
    if (lastSel != NO_SELECTION) {
        return meshes[lastSel]->isDrawSelection();
    }
    return false;
}

ll::Vector4 Model::getSelPoint() const {
    if (lastSel != NO_SELECTION) {
        return meshes[lastSel]->getSelVertex();
    }
    return {};
}

void Model::setSelPoint(const ll::Vector4& vec) {
    if (lastSel != NO_SELECTION) {
        meshes[lastSel]->setPos(vec);
    }
}

void Model::setDrawLines(bool isDraw) {
    for (const auto& mesh : meshes) {
        mesh->setDrawLines(isDraw);
    }
}

void Model::setDrawTriangles(bool drawTriangles) {
    for (const auto& mesh : meshes) {
        mesh->setDrawTriangles(drawTriangles);
    }
}

void Model::press(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection) {
    keyMods = modifiers;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 240 - currentMouseCameraPos.y, 1);
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    dragging = true;

    if ((keyMods & Qt::ShiftModifier) != 0) {
        if (!meshes.empty() && meshes[lastSel]->getCurrentSelection() != NO_SELECTION) {
            emit clickWithShift(currentMouseWorldPos);
            return;
        }
        meshes.push_back(std::make_unique<Mesh>(currentMouseWorldPos, ll::Color(0, 1, 0)));
        selectMesh(meshes.size() - 1, false);
        lastSel = meshes.size() - 1;
        connect(this, &Model::clickWithShift, meshes.back().get(), &Mesh::createNew);
        connect(this, &Model::click, meshes.back().get(), &Mesh::select);
        connect(this, &Model::presel, meshes.back().get(), &Mesh::preselect);
    } else {
        emit click();
        selectMesh(lastPresel, false);
        lastSel = lastPresel;
    }
}

void Model::pressRight() {
    if (lastPresel != NO_SELECTION) {
        meshes[lastPresel]->removeVertex();
        if (meshes[lastPresel]->getVertexes().size() == 0) {
            meshes.erase(meshes.begin() + lastPresel);
            lastSel = NO_SELECTION;
            lastPresel = NO_SELECTION;
        }
    }
}

void Model::move(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection) {
    keyMods = modifiers;
    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseWorldPos = viewProjection.inverse() * currentTranslatedMouseCameraPos;
    currentMouseCameraPos = pos;
    currentTranslatedMouseCameraPos = ll::Vector4::position(currentMouseCameraPos.x, 240 - currentMouseCameraPos.y, 1);

    if (dragging && isInside) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        auto pos = meshes[lastSel]->getSelVertex();
        auto newPos = pos + viewProjection.inverse() * ll::Vector4::direction(diff.x, -diff.y, 0);
        meshes[lastSel]->setPos(newPos.toHomogenous());
    }

    if ((keyMods & (Qt::ShiftModifier | Qt::ControlModifier)) != 0) {
        return;
    }

    emit presel(NO_SELECTION);

    float closestPoint = PIXEL_RADIUS;
    int closestPointIdx = -1;
    int closestMeshIdx = -1;

    for (int i = 0; i < meshes.size(); ++i) {
        auto curVerts = meshes[i]->getVertexes();
        for (int j = 0; j < curVerts.size(); ++j) {
            auto vector4 = viewProjection * curVerts[j];
            vector4.z = 0;
            float newPoint = (vector4 - currentTranslatedMouseCameraPos).normal();
            if (newPoint < closestPoint) {
                closestPoint = newPoint;
                closestPointIdx = j;
                closestMeshIdx = i;
            }
        }
    }

    if (closestPointIdx != -1) {
        isInside = true;
        selectMesh(closestMeshIdx, true);
        lastPresel = closestMeshIdx;
        emit presel(closestPointIdx);
    } else {
        isInside = false;
    }
}

void Model::release() {
    dragging = false;
}

void Model::selectMesh(int idx, bool presel) {
    for (int i = 0; i < meshes.size(); ++i) {
        if (i != idx) {
            if (presel) {
                meshes[i]->setDrawPreselection(false);
            } else {
                meshes[i]->setDrawSelection(false);
            }
        } else {
            if (presel) {
                meshes[i]->setDrawPreselection(true);
            } else {
                meshes[i]->setDrawSelection(true);
            }
        }
    }
}
