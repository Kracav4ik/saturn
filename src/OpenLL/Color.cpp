#include "Color.h"

#include <cmath>

using namespace ll;

namespace {

float colorClip(float c) {
    return std::max(0.f, std::min(c, 1.f));
}

}

static const float DISC = -1;

const Color Color::DISCARD = Color(DISC, 0, 0, 0);

Color::Color(float a, float r, float g, float b)
    : a(colorClip(a))
    , r(colorClip(r))
    , g(colorClip(g))
    , b(colorClip(b))
{
    if (a == DISC) {
        Color::a = DISC;
    }
}

Color::Color(float r, float g, float b)
    : Color(1, r, g, b)
{
}

Color::Color()
    : Color(0, 0, 0, 0)
{
}

bool Color::isDiscard() const {
    return a == DISC;
}
