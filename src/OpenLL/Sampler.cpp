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
    float u = std::max(0.f, std::min(uv.u, 1.f));
    float v = std::max(0.f, std::min(uv.v, 1.f));
    int ui = static_cast<int>(u*w);
    int vi = static_cast<int>(v*h);
    return img[vi * w + ui];

}
