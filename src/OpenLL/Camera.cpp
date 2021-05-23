#include "Camera.h"

using namespace ll;

Camera::Camera() {
    reset();
}

void Camera::move(float dx, float dy, float dz) {
    pos += Vector4::direction(dx, dy, dz);
    updateTransform();
}

void Camera::moveTo(const Vector4& newPos) {
    pos = newPos;
    updateTransform();
}

void Camera::rotateX(float da) {
    rotX += da;
    updateTransform();
}

void Camera::rotateY(float da) {
    rotY += da;
    updateTransform();
}

void Camera::reset() {
    pos = Vector4::position(0, 0, 0);
    rotX = 0;
    rotY = 0;
    updateTransform();
}

void Camera::updateTransform() {
    auto lookAt = pos + ll::Matrix4x4::rotY(rotY) * ll::Matrix4x4::rotX(rotX) * Vector4::direction(0, 0, -1);
    transform = Matrix4x4::lookAt(pos, lookAt, Vector4::direction(0, 1, 0));
}
