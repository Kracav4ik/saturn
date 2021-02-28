#pragma once

#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"

#include <QGraphicsView>

using DrawFunc = std::function<void(ll::DrawAPI& drawAPi, float angle)>;

class Frame : public QGraphicsView {
Q_OBJECT

public:
    explicit Frame(QWidget* parent);

    void drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 world, float angle);
    void setDrawer(DrawFunc drawFunc);
    void reset();

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    QPoint startDragPoint{0, 0};
    bool dragging = false;

    ll::Matrix4x4 fullRot;
    ll::Matrix4x4 currRot;

    DrawFunc draw;
    ll::DrawAPI drawAPi;
    ll::Framebuffer fb;
    QGraphicsScene scene;
};
