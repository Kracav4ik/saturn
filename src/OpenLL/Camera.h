#pragma once

#include "Vector4.h"
#include "Matrix4x4.h"

namespace ll {

class Camera {
public:
    Camera();

    void move(float dx, float dy, float dz);
    void moveTo(const Vector4& newPos);
    void rotateX(float da);
    void rotateY(float da);

    void reset();

    const Vector4& getPos() const { return pos; }
    const Matrix4x4& getTransform() const { return transform; }

private:
    void updateTransform();

    Vector4 pos;
    float rotX;
    float rotY;

    Matrix4x4 transform;
};

}
