#pragma once

#include "SelectedAndDraggable.h"

#include <vector>
#include <QObject>

class Polyline : public SAD {
public:
    explicit Polyline(const ll::Vector4& vertex, ll::Color color=ll::Color(0, 0, 0));
    Polyline();

    void addVertex(const ll::Vector4& pos) override;
    void draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const override;

    bool onBorder() const;

    void setPos(const ll::Vector4& pos) override;
    bool isDrawSelection() const override;

    void setDrawLines(bool drawLines);

private:
    bool isValid() const;
    bool isFirstSelected() const;
    bool isLastSelected() const;

    bool drawLines;
};
