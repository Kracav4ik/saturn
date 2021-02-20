#include "Sampler.h"
#include "Vector2.h"

using namespace ll;

Sampler::Sampler(const uint32_t* data, int width, int height)
    : w(width)
    , h(height)
{
    for (int i = 0; i < width * height; ++i) {
        auto val = data[i];
        img.emplace_back(((val & 0xff0000) >> 16) / 255.f, ((val & 0xff00) >> 8) / 255.f, (val & 0xff) / 255.f);
    }
}

Color ll::Sampler::getColor(const ll::Vector2& uv) const {
    int ui = static_cast<int>(uv.u*w);
    int vi = static_cast<int>(uv.v*h);
    ui = std::max(0, std::min(ui, w - 1));
    vi = std::max(0, std::min(vi, h - 1));
    return img[vi * w + ui];

}
