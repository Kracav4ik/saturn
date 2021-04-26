#include "LinesWithClipRect.h"

#include "OpenLL/Line.h"
#include "OpenLL/DrawAPI.h"

#include <iostream>

const int RECT_SELECT = -2;

LinesWithClipRect::LinesWithClipRect(ll::Vector4 rectPos, float rectWidth, float rectHeight)
        : rectPos(rectPos)
        , rectWidth(rectWidth)
        , rectHeight(rectHeight)
{
}

void LinesWithClipRect::draw(ll::DrawAPI& drawAPi, ll::Matrix4x4 viewProjection) const {
    SelectedAndDraggable::draw(drawAPi, viewProjection);

    std::vector<ll::Line> lines;

    auto bl = rectPos + ll::Vector4::direction(-rectWidth, -rectHeight, 0);
    auto br = rectPos + ll::Vector4::direction(rectWidth, -rectHeight, 0);
    auto tl = rectPos + ll::Vector4::direction(-rectWidth, rectHeight, 0);
    auto tr = rectPos + ll::Vector4::direction(rectWidth, rectHeight, 0);

    auto alterColor = ll::Color(1, 0, 0);

    auto pointInRect = [&](const ll::Vector4& pos) {
        return tl.x <= pos.x && pos.x <= br.x && br.y <= pos.y && pos.y <= tl.y;
    };
    
    for (int i = 1; i < vertexes.size(); i += 2) {
        auto from = vertexes[i - 1];
        auto to = vertexes[i];
        auto toInRect = pointInRect(to);
        auto fromInRect = pointInRect(from);

        auto diff = to - from;
        auto getPosFromX = [&](float x) {
            if (diff.x == 0) return ll::Vector4();
            auto a = diff.y / diff.x;
            auto b = to.y - a * to.x;
            return ll::Vector4::position(x, a * x + b, 0);
        };

        auto getPosFromY = [&](float y) {
            if (diff.y == 0) return ll::Vector4();
            auto a = diff.x / diff.y;
            auto b = to.x - a * to.y;
            return ll::Vector4::position(a * y + b, y, 0);
        };

        if (fromInRect && toInRect) {
            lines.push_back({{alterColor, from}, {alterColor, to}});
            continue;
        }
        if (fromInRect || toInRect) {
            if (toInRect) {
                std::swap(to, from);
                diff = to - from;
            }

            ll::Vector4 borderPos;

            if (diff.x > 0) {
                borderPos = getPosFromX(br.x);
            } else {
                borderPos = getPosFromX(tl.x);
            }

            if (pointInRect(borderPos) && (from.x <= borderPos.x && borderPos.x <= to.x || from.x >= borderPos.x && borderPos.x >= to.x)) {
                lines.push_back({{alterColor, from}, {alterColor, borderPos}});
                lines.push_back({{color, borderPos}, {color, to}});
                continue;
            }

            if (diff.y > 0) {
                borderPos = getPosFromY(tl.y);
            } else {
                borderPos = getPosFromY(br.y);
            }

            if (pointInRect(borderPos) && (from.y <= borderPos.y && borderPos.y <= to.y || from.y >= borderPos.y && borderPos.y >= to.y)) {
                lines.push_back({{alterColor, from}, {alterColor, borderPos}});
                lines.push_back({{color, borderPos}, {color, to}});
                continue;
            }
        }

        {
            if (from.x > to.x) {
                std::swap(from, to);
                diff = to - from;
            }

            if (from.x <= tl.x && tl.x <= to.x) {
                auto firstBordPos = getPosFromX(tl.x);
                ll::Vector4 secBordPos;
                if (pointInRect(firstBordPos)) {
                    if (diff.y > 0) {
                        secBordPos = getPosFromY(tl.y);
                    } else {
                        secBordPos = getPosFromY(br.y);
                    }
                    if (!pointInRect(secBordPos)) {
                        secBordPos = getPosFromX(br.x);
                    }

                    lines.push_back({{color, from}, {color, firstBordPos}});
                    lines.push_back({{alterColor, firstBordPos}, {alterColor, secBordPos}});
                    lines.push_back({{color, secBordPos}, {color, to}});
                    continue;
                }
            }

            if (from.x <= br.x && br.x <= to.x) {
                auto firstBordPos = getPosFromX(br.x);
                ll::Vector4 secBordPos;
                if (pointInRect(firstBordPos)) {
                    if (diff.y < 0) {
                        secBordPos = getPosFromY(tl.y);
                    } else {
                        secBordPos = getPosFromY(br.y);
                    }
                    if (!pointInRect(secBordPos)) {
                        secBordPos = getPosFromX(tl.x);
                    }

                    lines.push_back({{color, from}, {color, secBordPos}});
                    lines.push_back({{alterColor, secBordPos}, {alterColor, firstBordPos}});
                    lines.push_back({{color, firstBordPos}, {color, to}});
                    continue;
                }
            }
        }

        {
            if (from.y > to.y) {
                std::swap(from, to);
                diff = to - from;
            }

            if (from.y <= tl.y && tl.y <= to.y) {
                auto firstBordPos = getPosFromY(br.y);
                ll::Vector4 secBordPos;
                if (pointInRect(firstBordPos)) {
                    if (diff.x > 0) {
                        secBordPos = getPosFromX(br.x);
                    } else {
                        secBordPos = getPosFromX(tl.x);
                    }
                    if (!pointInRect(secBordPos)) {
                        secBordPos = getPosFromY(tl.y);
                    }


                    lines.push_back({{color, from}, {color, firstBordPos}});
                    lines.push_back({{alterColor, firstBordPos}, {alterColor, secBordPos}});
                    lines.push_back({{color, secBordPos}, {color, to}});
                    continue;
                }
            }
        }

        lines.push_back({{color, from}, {color, to}});
    }

    lines.push_back({{color, tr}, {color, tl}});
    lines.push_back({{color, tr}, {color, br}});
    lines.push_back({{color, br}, {color, bl}});
    lines.push_back({{color, bl}, {color, tl}});

    drawAPi.addShapes(lines);
}

void LinesWithClipRect::setRectPos(const ll::Vector4& newRectPos) {
    rectPos = newRectPos;
}

void LinesWithClipRect::setRectWidth(int newRectWidth) {
    rectWidth = newRectWidth;
}

void LinesWithClipRect::setRectHeight(int newRectHeight) {
    rectHeight = newRectHeight;
}
