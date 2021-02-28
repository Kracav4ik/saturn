#include "Frame.h"

#include <QDebug>
#include <QDragMoveEvent>

Frame::Frame(QWidget* parent)
    : QGraphicsView(parent)
    , fb(320, 240)
{
    reset();
}

void Frame::drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 world, float angle) {
    drawAPi.reset();

    drawAPi.clear(fb, ll::Color(0.5, 0.5, 0.5));
    drawAPi.pushMatrix(ll::Matrix4x4::toScreenSpace(fb.getW(), fb.getH()));

    drawAPi.pushMatrix(projection);

    drawAPi.pushMatrix(fullRot * currRot);

    drawAPi.pushMatrix(world);

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

void Frame::setDrawer(DrawFunc drawFunc) {
    draw = std::move(drawFunc);
}

void Frame::reset() {
    fullRot = ll::Matrix4x4::identity();
    currRot = ll::Matrix4x4::identity();
}

void Frame::mousePressEvent(QMouseEvent* event) {
    startDragPoint = event->pos();
    dragging = true;
}

void Frame::mouseMoveEvent(QMouseEvent* event) {
    if (dragging) {
        auto diff = event->pos() - startDragPoint;
        currRot = ll::Matrix4x4::rotY(diff.x() / 100.f) * ll::Matrix4x4::rotX(diff.y() / 100.f);
    }
}

void Frame::mouseReleaseEvent(QMouseEvent* event) {
    fullRot = fullRot * currRot;
    currRot = ll::Matrix4x4::identity();
    dragging = false;
}
