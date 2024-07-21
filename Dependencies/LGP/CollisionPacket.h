#pragma once
struct PhysicsComponent;

namespace LGP
{
    /// <summary>
    /// Contains all relevant information to resolve collision.
    /// </summary>
    struct CollisionPacket
    {
        /// <summary>
        /// Collision in world space
        /// </summary>
        glm::vec3 worldContact;

        /// <summary>
        /// Normal of collision, points from a to b
        /// </summary>
        glm::vec3 normal;

        /// <summary>
        /// Penertration depth of collision
        /// </summary>
        float depth;

        PhysicsComponent* objectA;
        PhysicsComponent* objectB;

        glm::vec3 aPos;
        glm::vec3 bPos;

        CollisionPacket()
        {
            depth = 0;
            worldContact = glm::zero<glm::vec3>();
            normal = glm::zero<glm::vec3>();
            objectA = nullptr;
            objectB = nullptr;
            aPos = { 0,0,0 };
            bPos = { 0,0,0 };
        }
    };
}