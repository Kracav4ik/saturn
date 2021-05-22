#pragma once

#include <functional>
#include <memory>

namespace ll {

class Color;
class Sampler;
struct Fragment;
struct Framebuffer;

using Shader = std::function<Color(const Fragment&, const Sampler*, std::shared_ptr<ll::Framebuffer>)>;

}
