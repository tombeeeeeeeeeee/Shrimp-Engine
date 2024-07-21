#pragma once
#include "config.h"
#include "Shape.h"

namespace LGP
{
    struct PhysicsComponent
    {
        glm::vec3 momentOfInertia;

        glm::vec3 force;
        glm::vec3 velocity;
        glm::vec3 netDepen;
        glm::mat3 invBodyIT;
        glm::mat3 invWorldIT;
        glm::vec3 torque;
        glm::vec3 angularVelocity;
        glm::vec3 angularMomentum;

        float dX, dY, dZ;
        std::vector<Shape> shapes;
        float invMass;
        glm::vec3 centreOfMass;
        bool isGravitated;
        float elasticCoef;
        float drag;
        float angularDrag;

        unsigned int collisionLayer = 0;
    };
}