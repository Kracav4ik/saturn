#pragma once

#include "SelectedAndDraggable.h"

class LinesWithClipRect : public SAD {
public:
    LinesWithClipRect(ll::Vector4 rectPos, float rectWidth, float rectHeight);

    void draw(ll::DrawAPI& y, ll::Matrix4x4 viewProjection) const override;

    void setRectPos(const ll::Vector4& newRectPos);
    void setRectWidth(float newRectWidth);
    void setRectHeight(float newRectHeight);

private:
    ll::Vector4 rectPos;
    float rectWidth;
    float rectHeight;
};
