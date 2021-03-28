#pragma once

#include "OpenLL/Matrix4x4.h"
#include "Polyline.h"

#include <memory>
#include <QObject>

using Polylines = std::vector<std::unique_ptr<Polyline>>;

namespace ll { class DrawAPI; };

class QMouseEvent;

class Model : public QObject{
Q_OBJECT
public:
    void draw(ll::DrawAPI& drawApi, ll::Matrix4x4 viewProjection);

public slots:
    void press(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 viewProjection);
    void move(const ll::Vector4& pos, bool withShift, ll::Matrix4x4 viewProjection);
    void release();

signals:
    void clickWithShift(const ll::Vector4& pos);
    void click(const ll::Vector4& pos);
    void presel(int idx);

private:
    void selectPolyline(int idx, bool presel);

    bool dragging = false;
    bool isInside = false;
    bool isShiftPressed = false;
    int lastPresel = NO_SELECTION;
    int lastSel = NO_SELECTION;

    ll::Vector4 currentMouseCameraPos;
    ll::Vector4 currentTranslatedMouseCameraPos;
    ll::Vector4 currentMouseWorldPos;
    Polylines polylines;
};
