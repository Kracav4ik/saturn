#include "Frame.h"

#include <QDebug>
#include <QDragMoveEvent>
#include <QGuiApplication>

void recallToFunc(const QPoint& pos, Frame* frame, void(Frame::*func)(const ll::Vector4&, bool, ll::Matrix4x4)) {
    auto curQuery = QGuiApplication::queryKeyboardModifiers();
    (frame->*func)(ll::Vector4::position(pos.x(), pos.y(), 0), (curQuery & Qt::ShiftModifier) != 0, frame->getViewProjection());
}

Frame::Frame(QWidget* parent)
    : QGraphicsView(parent)
    , fb(640, 480)
{
}

void Frame::drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 lookAt, ll::Matrix4x4 frameRot, float angle) {
    drawAPi.reset();

    drawAPi.clear(fb, ll::Color(0.7, 0.7, 0.7));

    viewProjection = ll::Matrix4x4::toScreenSpace(fb.getW(), fb.getH())
                     * projection
                     * lookAt
                     * ll::Matrix4x4::rotY(currRot.x() / 100.f) * ll::Matrix4x4::rotX(currRot.y() / 100.f)
                     * frameRot;

    drawAPi.pushMatrix(viewProjection);

    model->draw(drawAPi, viewProjection);

    init(drawAPi, isLight);
    draw(drawAPi, isLight, angle);

    drawAPi.drawFrame(fb);

    auto pixels = fb.getColorsARGB32();
    auto img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
    auto pm = QPixmap::fromImage(img);

    scene.clear();
    scene.addPixmap(pm);
    setScene(&scene);
    show();
}

ll::Matrix4x4 Frame::getViewProjection() const {
    return viewProjection;
}

void Frame::setIniter(InitFunc initFunc) {
    init = std::move(initFunc);
}

void Frame::setDrawer(DrawFunc drawFunc) {
    draw = std::move(drawFunc);
}

void Frame::setIsLight(bool light) {
    isLight = light;
}

void Frame::reset() {
    currRot = QPoint();
}

const ll::Framebuffer& Frame::getFb() {
    return fb;
}

void Frame::mousePressEvent(QMouseEvent* event) {
    dragging = true;
    if ((event->buttons() & Qt::LeftButton) != 0) {
        recallToFunc(event->pos(), this, &Frame::press);
    } else if ((event->buttons() & Qt::RightButton) != 0) {
        pressRight();
    }
}

void Frame::mouseMoveEvent(QMouseEvent* event) {
    static QPoint currentMouseCameraPos = event->pos();

    auto prevMouseCameraPos = currentMouseCameraPos;
    currentMouseCameraPos = event->pos();

    if (dragging && allowRot) {
        auto diff = currentMouseCameraPos - prevMouseCameraPos;
        currRot += diff;
    }

    recallToFunc(event->pos(), this, &Frame::move);
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
