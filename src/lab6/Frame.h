#pragma once

#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"

#include <QGraphicsView>

using DrawFunc = std::function<void(ll::DrawAPI& drawAPi, float elapsed)>;

namespace ll {
class Camera;
}

class Frame : public QGraphicsView {
Q_OBJECT

public:
    explicit Frame(QWidget* parent);

    void drawFrame(const ll::Color& bgColor, const ll::Matrix4x4& projection, const ll::Camera& camera, float elapsed);
    ll::Matrix4x4 getToScreen() const;
    ll::Matrix4x4 getViewProjection() const;
    void setDrawer(DrawFunc drawFunc);
    void setCullMode(ll::CullMode cullMode);
    const ll::Framebuffer& getFb();

    void setAllowDragging(bool allowDragging);

signals:
    void dragMove(int dx, int dy);

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    ll::Framebuffer fb;
    bool allowDrag;
    bool dragging;
    ll::Matrix4x4 toScreen;
    ll::Matrix4x4 viewProjection;
    DrawFunc draw;
    ll::DrawAPI drawAPi;
    QGraphicsScene scene;
};
