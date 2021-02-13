#include "Color.h"

using namespace ll;

Color::Color(float a, float r, float g, float b)
    : a(a), r(r), g(g), b(b)
{
}

Color::Color(float r, float g, float b)
    : Color(1, r, g, b)
{
}

Color::Color()
    : Color(0, 0, 0, 0)
{
}
