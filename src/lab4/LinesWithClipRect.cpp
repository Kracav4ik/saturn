#include "LinesWithClipRect.h"

#include "OpenLL/Line.h"
#include "OpenLL/DrawAPI.h"

#include <functional>

namespace {
struct Rect {
    ll::Vector4 topLeft;
    ll::Vector4 bottomRight;
};

struct Line {
    ll::Vector4 from;
    ll::Vector4 to;

    ll::Vector4 diff() {
        return to - from;
    }
};

const ll::Color MAIN_COLOR = ll::Color(0, 0, 1);
const ll::Color ALTER_COLOR = ll::Color(1, 0, 0);

}

template <auto ll::Vector4::*m>
bool valueBetween(const ll::Vector4& from, const ll::Vector4& to, const ll::Vector4& val) {
    return from.*m <= val.*m && val.*m <= to.*m || from.*m >= val.*m && val.*m >= to.*m;
}

bool pointInRect(const Rect& rect, const ll::Vector4& pos) {
    return valueBetween<&ll::Vector4::x>(rect.topLeft, rect.bottomRight, pos) && valueBetween<&ll::Vector4::y>(rect.topLeft, rect.bottomRight, pos);
}

template <auto ll::Vector4::*m, auto ll::Vector4::*n>
ll::Vector4 getPosFromVal(ll::Vector4 val, Line line) {
    auto diff = line.diff();
    if (diff.*m == 0) return ll::Vector4();

    auto a = diff.*n / diff.*m;
    auto b = line.to.*n - a * line.to.*m;
    auto valX = val.*m;
    auto valF = a * valX + b;
    if (m == &ll::Vector4::x) {
        return ll::Vector4::position(valX, valF, 0);
    }
    return ll::Vector4::position(valF, valX, 0);
};

template <auto ll::Vector4::*m>
bool checkInside(bool condition, std::vector<ll::Line>& lines, const Rect& rect, const Line& line, const std::function<ll::Vector4(ll::Vector4, Line)>& getPosFromFunc) {
    ll::Vector4 borderPos;

    if (condition) {
        borderPos = getPosFromFunc(rect.bottomRight, line);
    } else {
        borderPos = getPosFromFunc(rect.topLeft, line);
    }

    if (pointInRect(rect, borderPos) && valueBetween<m>(line.from, line.to, borderPos)) {
        lines.push_back({{ALTER_COLOR, line.from}, {ALTER_COLOR, borderPos}});
        lines.push_back({{MAIN_COLOR, borderPos}, {MAIN_COLOR, line.to}});
        return true;
    }
    return false;
}

template <auto ll::Vector4::*m, auto ll::Vector4::*n>
bool checkIntersects(bool condition, std::vector<ll::Line>& lines, const Rect& rect, const Line& line) {
    if (valueBetween<m>(line.from, line.to, rect.topLeft)) {
        auto firstBordPos = getPosFromVal<m, n>(rect.topLeft, line);
        if (pointInRect(rect, firstBordPos)) {
            ll::Vector4 secBordPos;
            if (condition) {
                secBordPos = getPosFromVal<n, m>(rect.topLeft, line);
            } else {
                secBordPos = getPosFromVal<n, m>(rect.bottomRight, line);
            }
            if (!pointInRect(rect, secBordPos)) {
                secBordPos = getPosFromVal<m, n>(rect.bottomRight, line);
            }

            lines.push_back({{MAIN_COLOR, line.from}, {MAIN_COLOR, firstBordPos}});
            lines.push_back({{ALTER_COLOR, firstBordPos}, {ALTER_COLOR, secBordPos}});
            lines.push_back({{MAIN_COLOR, secBordPos}, {MAIN_COLOR, line.to}});
            return true;
        }
    }

    return false;
}

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

    Rect clipRect = {tl, br};
    Rect backClipRect = {br, tl};

    for (int i = 1; i < vertexes.size(); i += 2) {
        auto from = vertexes[i - 1];
        auto to = vertexes[i];
        Line line = {from, to};
        auto toInRect = pointInRect(clipRect, to);
        auto fromInRect = pointInRect(clipRect, from);

        if (fromInRect && toInRect) {
            lines.push_back({{ALTER_COLOR, from}, {ALTER_COLOR, to}});
            continue;
        }
        if (fromInRect || toInRect) {
            if (toInRect) {
                std::swap(to, from);
                line = {from, to};
            }

            if (checkInside<&ll::Vector4::x>(line.diff().x > 0, lines, clipRect, line, getPosFromVal<&ll::Vector4::x, &ll::Vector4::y>)) {
                continue;
            }

            if (checkInside<&ll::Vector4::y>(line.diff().y < 0, lines, clipRect, line, getPosFromVal<&ll::Vector4::y, &ll::Vector4::x>)) {
                continue;
            }
        }

        {
            if (from.x > to.x) {
                std::swap(from, to);
                line = {from, to};
            }

            if (checkIntersects<&ll::Vector4::x, &ll::Vector4::y>(line.diff().y > 0, lines, clipRect, line)) {
                continue;
            }

            if (checkIntersects<&ll::Vector4::x, &ll::Vector4::y>(line.diff().y > 0, lines, backClipRect, {to, from})) {
                continue;
            }
        }

        {
            if (from.y > to.y) {
                std::swap(from, to);
                line = {from, to};
            }

            if (checkIntersects<&ll::Vector4::y, &ll::Vector4::x>(line.diff().x < 0, lines, backClipRect, line)) {
                continue;
            }
        }

        lines.push_back({{MAIN_COLOR, from}, {MAIN_COLOR, to}});
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

void LinesWithClipRect::setRectWidth(float newRectWidth) {
    rectWidth = newRectWidth;
}

void LinesWithClipRect::setRectHeight(float newRectHeight) {
    rectHeight = newRectHeight;
}
