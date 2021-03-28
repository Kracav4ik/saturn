#pragma once

#include "Model.h"
#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"

#include <QGraphicsView>

using DrawFunc = std::function<void(ll::DrawAPI& drawAPi, float angle)>;

class Frame : public QGraphicsView {
Q_OBJECT

public:
    explicit Frame(QWidget* parent);

    void drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 lookAt, ll::Matrix4x4 frameRot, float angle);
    ll::Matrix4x4 getViewProjection() const;
    void setIniter(DrawFunc initFunc);
    void setDrawer(DrawFunc drawFunc);
    void reset();

    void setAllowDragging(bool allowDragging);

    void setModel(const std::shared_ptr<Model>& newModel);

signals:
    void press(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 commonMatrix);
    void pressRight();
    void move(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 commonMatrix);
    void release();

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    bool allowRot = true;
    bool dragging = false;
    QPoint currRot;
    ll::Matrix4x4 viewProjection;
    std::shared_ptr<Model> model;
    DrawFunc init;
    DrawFunc draw;
    ll::DrawAPI drawAPi;
    ll::Framebuffer fb;
    QGraphicsScene scene;
};
