#pragma once

#include "Triangle.h"
#include "Matrix4x4.h"

#include <string>
#include <vector>

namespace ll {

class DrawAPI;
class Scene {
public:
    static Scene parseObj(const std::string& contents);
    void draw(DrawAPI& drawApi);
    void setTransform(const Matrix4x4& t);

private:
    struct SceneObject {
        std::string name;
        std::vector<Triangle> triangles;
    };

    std::vector<SceneObject> objects;
    Matrix4x4 transform;
};

}
