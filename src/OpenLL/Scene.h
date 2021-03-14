#pragma once

#include "Triangle.h"

#include <string>
#include <vector>

namespace ll {

class DrawAPI;
class Scene {
public:
    static Scene parseObj(const std::string& contents);
    void draw(DrawAPI& drawApi);

private:
    struct SceneObject {
        std::string name;
        std::vector<Triangle> triangles;
    };

    std::vector<SceneObject> objects;
};

}
