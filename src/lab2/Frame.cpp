#include "Frame.h"

#include <QDebug>
#include <QDragMoveEvent>
#include <QGuiApplication>

const int PIXEL_RADIUS = 10;

Frame::Frame(QWidget* parent)
    : QGraphicsView(parent)
    , fb(320, 240)
    , mouseRot(ll::Matrix4x4::identity())
{
    reset();
}

void Frame::drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 lookAt, ll::Matrix4x4 world, float angle) {
    drawAPi.reset();

    drawAPi.clear(fb, ll::Color(0.7, 0.7, 0.7));

    auto& vector = *polylines;
    if(QGuiApplication::queryKeyboardModifiers() && Qt::ShiftModifier != 0 && !allowRot) {
        isNewPoly = true;
        selectPolyline(NO_SELECTION, true);
        auto mouseColor = ll::Color(0, 1, 0);
        ll::Vertex mouseVert = {mouseColor, ll::Vector4::position(currentMouseCameraPos.x(), 240 - currentMouseCameraPos.y(), 1)};
        drawAPi.drawRound(
            mouseVert,
            PIXEL_RADIUS,
            false
        );

        if (lastSel != NO_SELECTION && vector[lastSel]->onBorder()) {
            drawAPi.addLines({ll::Line({mouseColor, commonMatrix * vector[lastSel]->getSelVertex()}, mouseVert)});
        }
    } else {
        isNewPoly = false;
    }

    commonMatrix = ll::Matrix4x4::toScreenSpace(fb.getW(), fb.getH())
                 * projection
                 * lookAt
                 * ll::Matrix4x4::rotY(currRot.x() / 100.f) * ll::Matrix4x4::rotX(currRot.y() / 100.f)
                 * world;

    drawAPi.pushMatrix(commonMatrix);

    draw(drawAPi, angle);

    for (const auto& polyline : vector) {
        polyline->draw(drawAPi);
    }

    drawAPi.drawFrame(fb);

    auto pixels = fb.getColorsARGB32();
    auto img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
    auto pm = QPixmap::fromImage(img);

    scene.clear();
    scene.addPixmap(pm);
    setScene(&scene);
    show();
}

void Frame::setDrawer(DrawFunc drawFunc) {
    draw = std::move(drawFunc);
}

void Frame::reset() {
    currRot = QPoint();
}

void Frame::mousePressEvent(QMouseEvent* event) {
    startDragPoint = event->pos();
    currentMouseCameraPos = startDragPoint;
    dragging = true;

    auto& vector = *polylines;
    if (isNewPoly) {
        if (!vector.empty() && vector.back()->onBorder()) {
            emit clickWithShift(currentMouseWorldPos);
            return;
        }
        vector.push_back(std::make_unique<Polyline>(currentMouseWorldPos, ll::Color(0, 0, 1)));
        selectPolyline(vector.size() - 1, false);
        lastSel = vector.size() - 1;
        connect(this, &Frame::clickWithShift, vector.back().get(), &Polyline::createNew);
        connect(this, &Frame::click, vector.back().get(), &Polyline::select);
        connect(this, &Frame::presel, vector.back().get(), &Polyline::preselect);
    } else {
        emit click();
        selectPolyline(lastPresel, false);
        lastSel = lastPresel;
    }
}

void Frame::mouseMoveEvent(QMouseEvent* event) {
    auto& vector = *polylines;

    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseCameraPos = event->pos();
    currentMouseWorldPos = commonMatrix.inverse() * mouseRot * ll::Vector4::position(currentMouseCameraPos.x(), 240 - currentMouseCameraPos.y(), 1);
    if (dragging) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        if (allowRot) {
            currRot += QPoint(diff.x(), diff.y());
        } else if (isInside) {
            auto pos = vector[lastSel]->getSelVertex();
            auto newPos = pos + commonMatrix.inverse() * ll::Vector4::direction(diff.x(), -diff.y(), 0);
            vector[lastSel]->setPos(newPos.toHomogenous());
        }
    }

    if (isNewPoly) {
        return;
    }

    emit presel(NO_SELECTION);

    float closestPoint = PIXEL_RADIUS;
    int closestPointIdx = -1;
    int closestPolylineIdx = -1;

    for (int i = 0; i < vector.size(); ++i) {
        auto curVerts = vector[i]->getVertexes();
        for (int j = 0; j < curVerts.size(); ++j) {
            auto vector4 = commonMatrix * curVerts[j];
            vector4.z = 0;
            float newPoint = (vector4 - ll::Vector4::position(currentMouseCameraPos.x(), 240 - currentMouseCameraPos.y(), 0)).normal();
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

void Frame::mouseReleaseEvent(QMouseEvent* event) {
    dragging = false;
}

void Frame::setAllowDragging(bool allowDrag) {
    allowRot = allowDrag;
}

void Frame::selectPolyline(int idx, bool presel) {
    auto& vector = *polylines;
    for (int i = 0; i < vector.size(); ++i) {
        if (i != idx) {
            if (presel) {
                vector[i]->setDrawPreselection(false);
            } else {
                vector[i]->setDrawSelection(false);
            }
        } else {
            if (presel) {
                vector[i]->setDrawPreselection(true);
            } else {
                vector[i]->setDrawSelection(true);
            }
        }
    }
}

void Frame::setPolylines(const std::shared_ptr<Polylines>& polys) {
    polylines = polys;
}

void Frame::setMouseRot(const ll::Matrix4x4& mRot) {
    mouseRot = mRot;
}
