#pragma once
#include <config.h>
#include "components/PhysicsComponent.h"
#include "components/TransformComponent.h"
#include "factories/SceneManager.h"

const float COLLISION_FACE_THRESHOLD = -0.0025f;

enum GJKEvolution
{
    evolving = 0,
    intersecting = 1,
    notIntersecting = 2
};

struct CollisionPacket
{
    glm::vec3 worldContact;
    glm::vec3 normal;
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
        aPos = {0,0,0};
        bPos = {0,0,0};
    }
};

class PhysicsSystem
{
private:
    std::vector<CollisionPacket> collisions;

    void ShapeCollisionCheck(PhysicsComponent& aPhysics, const TransformComponent& aTransform, 
        PhysicsComponent& bPhysics, const TransformComponent& bTransform);

    CollisionPacket GJK(Shape a, Shape b, glm::mat4 aTransform, glm::mat4 bTransform);
 
    std::vector<glm::vec3> ToVector(Shape shape);

    glm::vec3 CalculateSupport(std::vector<glm::vec3>& a, float aRadius, std::vector<glm::vec3>& b, float bRadius, glm::vec3 direction);

    glm::vec3 SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius);

    glm::vec3 SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius, int& index);

    GJKEvolution AddSupportToSimplex(std::vector<glm::vec3>& simp, glm::vec3 vert, glm::vec3 dir);

    GJKEvolution EvolveSimplex(std::vector<glm::vec3>& simp, glm::vec3& direction);

    void EPA(std::vector<glm::vec3>& simp, CollisionPacket& collision, 
        std::vector<glm::vec3>& aVerts, float aRadius, glm::mat4 aTransform, 
        std::vector<glm::vec3>& bVerts, float bRadius, glm::mat4 bTransform);

    std::vector<glm::vec4> GetFaceNormals(std::vector<glm::vec3>& polytope, std::vector<int> faces, int& face);

    void AddIfUniqueEdge(std::vector<std::pair<int, int>>& uniqueEdges, std::vector<int> edge, int aVert, int bVert);

    void CalculateCollsionPoint(std::vector<glm::vec3>& aVerts, Shape& aShape,
        std::vector<glm::vec3>& bVerts, Shape& bShape, glm::vec3 normal, glm::vec3& contactPoint);

    void AddFaceVert(glm::vec3 normal, int originIndex, int currIndex, std::vector<glm::vec3>& worldSpace, Shape shape, std::vector<int> verts);

    void Resolution(CollisionPacket& collision);

    glm::vec3 AddDepen(glm::vec3 newDepen, glm::vec3 currDepen);

    void Integration(PhysicsComponent& body, TransformComponent& transform, float deltaTime);

    glm::quat OrthonormalizeOrientation(const glm::mat4 rotation);

public:
    glm::vec3 gravity = { 0,0,0 };
    
    void CollisionPhase(std::unordered_map<unsigned int, PhysicsComponent>& bodies,
        std::unordered_map<unsigned int, TransformComponent>& transforms);


    void IntegrationStep(std::unordered_map<unsigned int, PhysicsComponent>& bodies,
        std::unordered_map<unsigned int, TransformComponent>& transforms, SceneManager* scene, float deltaTime);
};

