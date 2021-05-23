#pragma once

#include "Shader.h"

namespace ll {

struct Fragment;
class Framebuffer;
struct Matrices;
enum class CullMode;

struct Shape {
    virtual std::vector<Fragment> getFragments(Framebuffer& fb, const Matrices& matrices, CullMode cull) const = 0;
    virtual ~Shape() = default;
};

}
