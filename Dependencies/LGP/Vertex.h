#pragma once
#include "config.h"

namespace LGP
{
    struct Vertex
    {
        glm::vec3 vert = glm::zero<glm::vec3>();
        std::vector<int> edges;
    };
}