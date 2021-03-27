#pragma once

#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"
#include "Polyline.h"

#include <QGraphicsView>

using Polylines = std::vector<std::unique_ptr<Polyline>>;
using DrawFunc = std::function<void(ll::DrawAPI& drawAPi, float angle)>;

class Frame : public QGraphicsView {
Q_OBJECT

public:
    explicit Frame(QWidget* parent);

    void drawFrame(ll::Matrix4x4 projection, ll::Matrix4x4 lookAt, ll::Matrix4x4 world, float angle);
    void setDrawer(DrawFunc drawFunc);
    void reset();

    void setAllowDragging(bool allowDragging);
    void setPolylines(const std::shared_ptr<Polylines>& polylines);
    void setMouseRot(const ll::Matrix4x4& mouseRot);

signals:
    void click();
    void clickWithShift(const ll::Vector4& pos);
    void presel(int idx);

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void selectPolyline(int idx, bool presel);

    QPoint startDragPoint{0, 0};
    bool dragging = false;
    bool allowRot = true;
    bool isNewPoly = false;
    bool isInside = false;
    int lastPresel = NO_SELECTION;
    int lastSel = NO_SELECTION;

    QPoint currRot;
    ll::Matrix4x4 commonMatrix;
    ll::Matrix4x4 mouseRot;

    DrawFunc draw;
    ll::DrawAPI drawAPi;
    ll::Framebuffer fb;
    QGraphicsScene scene;
    ll::Vector4 currentMouseWorldPos;
    QPoint currentMouseCameraPos;
    std::shared_ptr<Polylines> polylines;
};
