#include "Frame.h"

#include <QDebug>
#include <QDragMoveEvent>
#include <QGuiApplication>
#include "OpenLL/Camera.h"

Frame::Frame(QWidget* parent)
    : QGraphicsView(parent)
    , fb(640, 480)
    , allowDrag(true)
    , dragging(false)
    , toScreen(ll::Matrix4x4::toScreenSpace(fb.getW(), fb.getH()))
{
}

void Frame::drawFrame(const ll::Color& bgColor, const ll::Matrix4x4& projection, const ll::Camera& camera, float elapsed) {
    drawAPi.reset();

    drawAPi.clear(fb, bgColor);

    viewProjection = projection * camera.getTransform();

    drawAPi.setToScreenMatrix(toScreen);
    drawAPi.setViewProjectionMatrix(viewProjection);

    draw(drawAPi, elapsed);

    drawAPi.drawFrame(fb);

    auto pixels = fb.getColorsARGB32();
    auto img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
    auto pm = QPixmap::fromImage(img);

    scene.clear();
    scene.addPixmap(pm);
    setScene(&scene);
    show();
}

ll::Matrix4x4 Frame::getToScreen() const {
    return toScreen;
}

ll::Matrix4x4 Frame::getViewProjection() const {
    return viewProjection;
}

void Frame::setDrawer(DrawFunc drawFunc) {
    draw = std::move(drawFunc);
}

void Frame::setCullMode(ll::CullMode cullMode) {
    drawAPi.setCullMode(cullMode);
}

const ll::Framebuffer& Frame::getFb() {
    return fb;
}

void Frame::mousePressEvent(QMouseEvent* event) {
    dragging = true;
}

void Frame::mouseMoveEvent(QMouseEvent* event) {
    static QPoint currentMouseCameraPos = event->pos();

    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseCameraPos = event->pos();

    if (dragging && allowDrag) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        emit dragMove(diff.x(), diff.y());
    }
}

void Frame::mouseReleaseEvent(QMouseEvent* event) {
    dragging = false;
}

void Frame::setAllowDragging(bool allow) {
    allowDrag = allow;
}
