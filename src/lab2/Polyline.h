#pragma once

#include "OpenLL/Vector4.h"
#include "OpenLL/Matrix4x4.h"
#include "OpenLL/Color.h"

#include <vector>
#include <QObject>

namespace ll { class DrawAPI; }

extern const int NO_SELECTION;
extern const float RADIUS;
extern const int PIXEL_RADIUS;

class Polyline : public QObject {
Q_OBJECT
public:
    explicit Polyline(const ll::Vector4& vertex, ll::Color color=ll::Color(0, 0, 0));
    Polyline();

    void addVertex(const ll::Vector4& pos);
    void removeVertex();
    bool onBorder() const;

    void setPos(const ll::Vector4& pos);
    void changeCurrentSelection(int newSelection);
    void changeColor(ll::Color newColor);
    int getCurrentSelection() const;
    int getCurrentPreselection() const;
    bool isDrawSelection() const;

    const std::vector<ll::Vector4>& getVertexes() const;
    ll::Vector4 getSelVertex() const;

    void draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const;

    void setDrawSelection(bool drawSelection);
    void setDrawPreselection(bool drawPresel);
    void setDrawLines(bool drawLines);

public slots:
    void select();
    void createNew(const ll::Vector4& pos);
    void preselect(int idx);

private:
    bool isValid() const;
    bool isFirstSelected() const;
    bool isLastSelected() const;

    std::vector<ll::Vector4> vertexes;
    bool drawLines;
    bool drawSelection;
    bool drawPreselection;
    int currentPreselection;
    int currentSelection;
    ll::Color color;
};
