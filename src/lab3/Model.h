#pragma once

#include "OpenLL/Matrix4x4.h"
#include "Mesh.h"

#include <memory>
#include <QObject>

class Mesh;

using Meshes = std::vector<std::unique_ptr<Mesh>>;

namespace ll { class DrawAPI; };

class QMouseEvent;

class Model : public QObject{
Q_OBJECT
public:
    void draw(ll::DrawAPI& drawApi, ll::Matrix4x4 viewProjection);

    bool getLastSelVis() const;
    ll::Vector4 getSelPoint() const;
    void setSelPoint(const ll::Vector4& vec);
    void setDrawLines(bool isDraw);
    void setDrawTriangles(bool drawTriangles);

public slots:
    void press(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection);
    void pressRight();
    void move(const ll::Vector4& pos, Qt::KeyboardModifiers modifiers, ll::Matrix4x4 viewProjection);
    void release();

signals:
    void clickWithShift(const ll::Vector4& pos);
    void click();
    void presel(int idx);

private:
    void selectMesh(int idx, bool presel);

    bool dragging = false;
    bool isInside = false;
    Qt::KeyboardModifiers keyMods;
    int lastPresel = NO_SELECTION;
    int lastSel = NO_SELECTION;

    ll::Vector4 currentMouseCameraPos;
    ll::Vector4 currentTranslatedMouseCameraPos;
    ll::Vector4 currentMouseWorldPos;
    Meshes meshes;
};
