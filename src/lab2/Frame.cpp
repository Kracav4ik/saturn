#include "Frame.h"

#include <QDebug>
#include <QDragMoveEvent>
#include <QGuiApplication>

Frame::Frame(QWidget* parent)
    : QGraphicsView(parent)
    , fb(320, 240)
{
}

void Frame::drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 lookAt, ll::Matrix4x4 frameRot, float angle) {
    drawAPi.reset();

    drawAPi.clear(fb, ll::Color(0.7, 0.7, 0.7));

    init(drawAPi, angle);

    viewProjection = ll::Matrix4x4::toScreenSpace(fb.getW(), fb.getH())
                     * projection
                     * lookAt
                     * ll::Matrix4x4::rotY(currRot.x() / 100.f) * ll::Matrix4x4::rotX(currRot.y() / 100.f)
                     * frameRot;

    drawAPi.pushMatrix(viewProjection);

    model->draw(drawAPi, viewProjection);

    draw(drawAPi, angle);

    drawAPi.drawFrame(fb);

    auto pixels = fb.getColorsARGB32();
    auto img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
    auto pm = QPixmap::fromImage(img);

    scene.clear();
    scene.addPixmap(pm);
    setScene(&scene);
    show();
}

void Frame::setIniter(DrawFunc initFunc) {
    init = std::move(initFunc);
}

void Frame::setDrawer(DrawFunc drawFunc) {
    draw = std::move(drawFunc);
}

void Frame::reset() {
    currRot = QPoint();
}

void Frame::mousePressEvent(QMouseEvent* event) {
    dragging = true;
    auto curQuery = QGuiApplication::queryKeyboardModifiers();
    auto point = event->pos();
    press(ll::Vector4::position(point.x(), point.y(), 0), (curQuery & Qt::ShiftModifier) != 0, viewProjection);
}

void Frame::mouseMoveEvent(QMouseEvent* event) {
    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseCameraPos = event->pos();

    if (dragging) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        if (allowRot) {
            currRot += diff;
        }
    }

    auto curQuery = QGuiApplication::queryKeyboardModifiers();
    auto point = event->pos();
    move(ll::Vector4::position(point.x(), point.y(), 0), (curQuery & Qt::ShiftModifier) != 0, viewProjection);
}

void Frame::mouseReleaseEvent(QMouseEvent* event) {
    dragging = false;
    release();
}

void Frame::setAllowDragging(bool allowDrag) {
    allowRot = allowDrag;
}

void Frame::setModel(const std::shared_ptr<Model>& newModel) {
    model = newModel;
}

