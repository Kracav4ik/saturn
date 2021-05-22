#pragma once

#include <functional>

namespace ll {

class Color;
class Sampler;
struct Fragment;

using Shader = std::function<Color(const Fragment&, const Sampler*)>;

}
