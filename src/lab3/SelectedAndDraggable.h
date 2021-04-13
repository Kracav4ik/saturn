#pragma once

#include "OpenLL/Color.h"
#include "OpenLL/Matrix4x4.h"

#include <vector>
#include <QObject>

namespace ll {
class DrawAPI;
class Vector4;
}

extern const int NO_SELECTION;
extern const float RADIUS;
extern const int PIXEL_RADIUS;

class SelectedAndDraggable : public QObject {
Q_OBJECT

public:
    explicit SelectedAndDraggable(const ll::Vector4& vertex, ll::Color color=ll::Color(0, 0, 0));
    SelectedAndDraggable();

    virtual void addVertex(const ll::Vector4& pos);
    virtual void removeVertex();

    void changeCurrentSelection(int newSelection);
    void changeColor(ll::Color newColor);

    virtual void draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const;
    virtual void setPos(const ll::Vector4& pos);
    virtual int getCurrentSelection() const;
    virtual int getCurrentPreselection() const;
    virtual bool isDrawSelection() const;

    const std::vector<ll::Vector4>& getVertexes() const;
    ll::Vector4 getSelVertex() const;

    void setDrawSelection(bool drawSel);
    void setDrawPreselection(bool drawPresel);

    const ll::Color& getColor() const;

public slots:
    void select();
    void createNew(const ll::Vector4& pos);
    void preselect(int idx);

protected:
    std::vector<ll::Vector4> vertexes;
    bool drawSelection;
    bool drawPreselection;
    int currentPreselection;
    int currentSelection;
    ll::Color color;

private:
    void drawSelPlesel(ll::DrawAPI& drawAPi, const ll::Matrix4x4& viewProjection) const;

};

using SAD = SelectedAndDraggable;
