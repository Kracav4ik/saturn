#pragma once

#include "Shader.h"

namespace ll {

struct Fragment;
class Framebuffer;
struct Matrix4x4;
enum class CullMode;

struct Shape {
    virtual std::vector<Fragment> getFragments(Framebuffer& fb, const Matrix4x4& transform, CullMode cull) const = 0;
    virtual ~Shape() = default;
};

}
