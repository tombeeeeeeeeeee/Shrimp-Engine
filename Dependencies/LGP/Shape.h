#pragma once
#include "config.h"
#include "Vertex.h"

namespace LGP
{
    struct Shape
    {
        std::vector<Vertex> vertices;
        float radius = 0;
    };
}