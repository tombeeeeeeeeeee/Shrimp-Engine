#pragma once
#include <config.h>
#include "components/PhysicsComponent.h"
#include "components/TransformComponent.h"
#include "factories/SceneManager.h"

/// <summary>
/// The threshold to which a vert will be registered as a colliding face (eg. vert * normal > threshold)
/// </summary>
const float COLLISION_FACE_THRESHOLD = -0.005f;

/// <summary>
/// An enum to help track the progress of GJK
/// </summary>
enum GJKEvolution
{
    evolving = 0,
    intersecting = 1,
    notIntersecting = 2
};

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
        aPos = {0,0,0};
        bPos = {0,0,0};
    }
};

class PhysicsSystem
{
private:

    /// <summary>
    /// Collisions calculated this frame, clears at the end of the resolution step.
    /// </summary>
    std::vector<CollisionPacket> collisions;

    /// <summary>
    /// Check the collision of each shape from the net shapes of two physics objects
    /// </summary>
    void ShapeCollisionCheck(PhysicsComponent& aPhysics, const TransformComponent& aTransform, 
        PhysicsComponent& bPhysics, const TransformComponent& bTransform);

    /// <summary>
    /// Gilbert Johnson Keerthi algorithm
    /// Checks if two shapes are colliding.
    /// Then performs EPA and calculates the collision point.
    /// </summary>
    /// <returns> a complete collision packet</returns>
    CollisionPacket GJK(Shape a, Shape b, glm::mat4 aTransform, glm::mat4 bTransform);
 
    /// <summary>
    /// Takes a shape and returns it as a list
    /// </summary>
    std::vector<glm::vec3> ToVector(Shape shape);

    /// <summary>
    /// Calculates furthest point in direction in a Minkowski Difference
    /// </summary>
    glm::vec3 CalculateSupport(std::vector<glm::vec3>& a, float aRadius, std::vector<glm::vec3>& b, float bRadius, glm::vec3 direction);

    /// <summary>
    /// Calculates furthest point in direction in a shape
    /// </summary>
    glm::vec3 SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius);

    /// <summary>
    /// Calculates furthest point in direction in a Minkowski Difference
    /// </summary>
    /// <param name="index"> index in shape of vert</param>
    glm::vec3 SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius, int& index);

    /// <summary>
    /// Add new vert to simplex
    /// </summary>
    /// <param name="simp"> current simplex</param>
    /// <param name="vert"> new vertex</param>
    /// <param name="dir"> direction of origin</param>
    /// <returns> whether the simplex still needs to evolve</returns>
    GJKEvolution AddSupportToSimplex(std::vector<glm::vec3>& simp, glm::vec3 vert, glm::vec3 dir);

    /// <summary>
    /// Main GJK loop, evolves simplex until it can be deteremined if origin is within it.
    /// </summary>
    GJKEvolution EvolveSimplex(std::vector<glm::vec3>& simp, glm::vec3& direction);

    /// <summary>
    /// Expanding Polytope Algorithm
    /// Determines the resposnible face for collision, and the penertrating depth from the collision.
    /// </summary>
    void EPA(std::vector<glm::vec3>& simp, CollisionPacket& collision, 
        std::vector<glm::vec3>& aVerts, float aRadius, glm::mat4 aTransform, 
        std::vector<glm::vec3>& bVerts, float bRadius, glm::mat4 bTransform);

    /// <summary>
    ///  Gets normals from current faces of the polytope.
    /// </summary>
    /// <param name="face"> face index with smallest depth</param>
    /// <returns> returns list of normals, where the w value is the depth along that normal</returns>
    std::vector<glm::vec4> GetFaceNormals(std::vector<glm::vec3>& polytope, std::vector<int> faces, int& face);

    /// <summary>
    /// Adds edge to polytope if its unique
    /// </summary>
    void AddIfUniqueEdge(std::vector<std::pair<int, int>>& uniqueEdges, std::vector<int> edge, int aVert, int bVert);

    /// <summary>
    /// Calculate collision point based off of the contact manifold.
    /// </summary>
    /// <param name="contactPoint"> reference to return to </param>
    void CalculateCollsionPoint(std::vector<glm::vec3>& aVerts, Shape& aShape,
        std::vector<glm::vec3>& bVerts, Shape& bShape, glm::vec3 normal, glm::vec3& contactPoint);

    /// <summary>
    /// Recursive function that checks all neighbouring verts to see if they are on the face responsible for collision.
    /// </summary>
    /// <param name="normal"> collision normal</param>
    /// <param name="originIndex"> index of first collision</param>
    /// <param name="currIndex"> index we are checking</param>
    /// <param name="worldSpace"> verts in world space</param>
    /// <param name="shape"> shape you are checking</param>
    /// <param name="verts"> verts so far</param>
    void AddFaceVert(glm::vec3 normal, int originIndex, int currIndex, std::vector<glm::vec3>& worldSpace, Shape shape, std::vector<int>& verts);

    /// <summary>
    /// Resolve Collision
    /// </summary>
    void Resolution(CollisionPacket& collision);

    /// <summary>
    /// Add depenertration for body.
    /// Determines if current depenertration encompasses any of the new depenertration.
    /// </summary>
    /// <param name="newDepen"> new addition depen</param>
    /// <param name="currDepen"> current depen</param>
    /// <returns> overwrite for current depen</returns>
    glm::vec3 AddDepen(glm::vec3 newDepen, glm::vec3& currDepen);

    /// <summary>
    /// Integration step, updates all properties of the physics body
    /// </summary>
    void Integration(PhysicsComponent& body, TransformComponent& transform, float deltaTime);

    /// <summary>
    /// takes a broken rotation matrix and asserts it to a quat
    /// </summary>
    /// <param name="rotation"></param>
    /// <returns></returns>
    glm::quat OrthonormalizeOrientation(const glm::mat4 rotation);

public:

    /// <summary>
    /// gravity to apply to gravitated bodies
    /// </summary>
    glm::vec3 gravity = { 0,0,0 };
    
    /// <summary>
    /// Enitre collision phase, checks all bodies to see if they are colliding
    /// </summary>
    /// <param name="bodies"> all bodies to check collision with</param>
    /// <param name="transforms"> transforms of the bodies</param>
    void CollisionPhase(std::unordered_map<unsigned int, PhysicsComponent>& bodies,
        std::unordered_map<unsigned int, TransformComponent>& transforms);

    /// <summary>
    /// Integration step for all bodies
    /// </summary>
    /// <param name="bodies"> all bodies to check collision with</param>
    /// <param name="transforms"> transforms of the bodies</param>
    /// <param name="scene"> current scene</param>
    void IntegrationStep(std::unordered_map<unsigned int, PhysicsComponent>& bodies,
        std::unordered_map<unsigned int, TransformComponent>& transforms, SceneManager* scene, float deltaTime);
};

