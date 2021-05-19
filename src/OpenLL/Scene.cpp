#include "Scene.h"
#include "DrawAPI.h"

#include <regex>

using namespace ll;

std::vector<std::string> split(const std::string& input, const std::string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first(input.begin(), input.end(), re, -1);
    return {first, std::sregex_token_iterator()};
}

Scene Scene::parseObj(const std::string& contents) {
    Scene result;
    
    std::vector<std::string> lines;

    auto begin = contents.begin();
    auto end = contents.end();
    while (true) {
        auto nextBeg = std::find(begin, end, '\n');

        if (*begin != '#') {
            auto newEnd = std::find(begin, nextBeg, '#');
            for (; newEnd != begin; --newEnd) {
                if (*(newEnd - 1) != ' ') {
                    break;
                }
            }
            if (begin != newEnd) {
                lines.emplace_back(begin, newEnd);
            }
        }
        if (nextBeg == end) {
            break;
        }
        begin = nextBeg + 1;
    }

    std::vector<Vector4> vertexes;
    std::vector<Vector2> UVs;
    Color color;

    // TODO: hardcode from materials file low-poly-mill.mtl
    std::unordered_map<std::string, Color> colors {
        {"facade1", Color(0.81033653020859, 0.75712728500366, 0.8265306353569)},
        {"facade", Color(0.57142859697342, 0.35390767455101, 0.1875681579113)},
        {"ground", Color(0.64285713434219, 0.43904036283493, 0.2061068713665)},
        {"platform", Color(0.00599781889468, 0.78571426868439, 0.00599781889468)},
        {"wood", Color(0.32653060555458, 0.16949680447578, 0.01246300060302)},
        {"foliage", Color(0, 0.70408165454864, 0.2581632733345)},
    };

    std::unordered_map<std::string, std::function<void(const std::string&)>> commands {
        {"mtllib", [&](const std::string& toParse) {}},
        {"usemtl", [&](const std::string& toParse) {
            color = colors[toParse];
        }},
        {"vn", [&](const std::string& toParse) {}},
        {"o", [&](const std::string& toParse) {
            result.objects.push_back({toParse, {}});
        }},
        {"v", [&](const std::string& toParse) {
            auto coords = split(toParse, " ");
            float scale = 0.01;
            vertexes.push_back({std::stof(coords[0]) * scale, std::stof(coords[1]) * scale, std::stof(coords[2]) * scale, 1});
        }},
        {"vt", [&](const std::string& toParse) {
            auto coords = split(toParse, " ");
            UVs.push_back({std::stof(coords[0]), std::stof(coords[1])});
        }},
        {"f", [&](const std::string& toParse) {
            auto& triangles = result.objects.back().triangles;
            auto verts = split(toParse, " ");
            std::vector<int> indexes;
            for (const auto& vert : verts) {
                indexes.push_back(std::stoi(split(vert, "/")[0]) - 1);
            }
            triangles.emplace_back(Triangle(
                {color, vertexes[indexes[0]]},
                {color, vertexes[indexes[1]]},
                {color, vertexes[indexes[2]]}
            ));

            if (indexes.size() > 3) {
                triangles.emplace_back(Triangle(
                    {color, vertexes[indexes[0]]},
                    {color, vertexes[indexes[2]]},
                    {color, vertexes[indexes[3]]}
                ));
            }
        }},
    };

    for (auto line : lines) {
        auto comEnd = std::find(line.begin(), line.end(), ' ');
        std::string command(line.begin(), comEnd);
        commands[command]({comEnd + 1, line.end()});
    }
    
    return result;
}

void Scene::draw(DrawAPI& drawApi) {
    for (const auto& object : objects) {
        drawApi.addShapes(object.triangles);
    }
}
