#include "PhysicsSystem.h"

void PhysicsSystem::CollisionPhase(std::unordered_map<unsigned int, PhysicsComponent>& bodies, 
	std::unordered_map<unsigned int, TransformComponent>& transforms)
{
	for (int CollisionCheckItterations = 0; CollisionCheckItterations < 10; CollisionCheckItterations++)
	{
		//BroadPhase
		std::unordered_map<unsigned int, PhysicsComponent>::iterator aIter = bodies.begin();
		if (aIter != bodies.end());
		{
			std::unordered_map<unsigned int, PhysicsComponent>::iterator bIter;
			for (aIter; aIter != bodies.end(); aIter++)
			{
				for (bIter = std::next(aIter); bIter != bodies.end(); bIter++)
				{
					//Narrow Phase
					ShapeCollisionCheck(aIter->second, transforms[aIter->first], bIter->second, transforms[bIter->first]);
				}
			}
		}
	}

    for (int i = 0; i < collisions.size(); i++)
    {
        Resolution(collisions[i]);
    }
    collisions.clear();
}

void PhysicsSystem::IntegrationStep(std::unordered_map<unsigned int, PhysicsComponent>& bodies, 
    std::unordered_map<unsigned int, TransformComponent>& transforms, 
    SceneManager* scene, float deltaTime)
{
    std::unordered_map<unsigned int, PhysicsComponent>::iterator iter = bodies.begin();
    for (iter; iter != bodies.end(); iter++)
    {
        Integration(iter->second, transforms[iter->first], deltaTime);
        scene->GlobalTransformUpdate(iter->first);
    }
}

void PhysicsSystem::ShapeCollisionCheck(PhysicsComponent& aPhysics, const TransformComponent& aTransform, 
	PhysicsComponent& bPhysics, const TransformComponent& bTransform)
{
	for (int i = 0; i < aPhysics.shapes.size(); i++)
	{
		for (int j = 0; j < bPhysics.shapes.size(); j++)
		{
			CollisionPacket collision = GJK(aPhysics.shapes[i], bPhysics.shapes[j], 
				aTransform.globalTransform, bTransform.globalTransform);
			collision.objectA = &aPhysics;
			collision.objectB = &bPhysics;
            if (collision.depth > 0)
            {
                collision.aPos = aTransform.globalTransform * glm::vec4(0, 0, 0, 1);
                collision.bPos = bTransform.globalTransform * glm::vec4(0, 0, 0, 1);
                collisions.push_back(collision);
            }
		}
	}
}

CollisionPacket PhysicsSystem::GJK(Shape a, Shape b, glm::mat4 aTransform, glm::mat4 bTransform)
{
    CollisionPacket collision;
    std::vector<glm::vec3> simp;
    std::vector<glm::vec3> shapeA, shapeB;
    shapeA = ToVector(a);
    shapeB = ToVector(b);

    for (int i = 0; i < shapeA.size(); i++)
    {
        shapeA[i] = aTransform * glm::vec4(shapeA[i], 1);
    }

    for (int i = 0; i < shapeB.size(); i++)
    {
        shapeB[i] = bTransform * glm::vec4(shapeB[i], 1);
    }

    //Collision Check
    glm::vec3 direction = {1,0,0};
    glm::vec3 support = CalculateSupport(shapeA, a.radius, shapeB, b.radius, direction);
    GJKEvolution gjking = AddSupportToSimplex(simp, support, direction);
    direction = -support;
    int iter = 0;
    while (gjking == GJKEvolution::evolving && iter < 100)
    {
        iter++;
        support = CalculateSupport(shapeA, a.radius, shapeB, b.radius, direction);
        if (AddSupportToSimplex(simp, support, direction) == GJKEvolution::evolving)
        {
            gjking = EvolveSimplex(simp, direction);
        }
        else
        {
            gjking = GJKEvolution::notIntersecting;
        }
    }

    if (gjking == GJKEvolution::intersecting)
    {
        EPA(simp, collision, shapeA, a.radius, aTransform, shapeB, b.radius, bTransform);
        CalculateCollsionPoint(shapeA, a, shapeB, b, collision.normal, collision.worldContact);
    }

    return collision;
}

std::vector<glm::vec3> PhysicsSystem::ToVector(Shape shape)
{
    std::vector<glm::vec3> verts;
    verts.reserve(shape.vertices.size());
    for (int i = 0; i < shape.vertices.size(); i++)
    {
        verts.push_back(shape.vertices[i].vert);
    }
    return verts;
}

glm::vec3 PhysicsSystem::CalculateSupport(std::vector<glm::vec3>& a, float aRadius, std::vector<glm::vec3>& b, float bRadius, glm::vec3 direction)
{
    return SupportFunction(direction, a, aRadius) - SupportFunction(-direction, b, bRadius);
}

glm::vec3 PhysicsSystem::SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius)
{
    float max = -FLT_MAX;
    int index = 0;
    for (int i = 0; i < verts.size(); i++)
    {
        float dot = glm::dot(verts[i], dir);
        if (dot > max)
        {
            max = dot;
            index = i;
        }
    }
    return verts[index] + glm::normalize(dir) * radius;
}

glm::vec3 PhysicsSystem::SupportFunction(glm::vec3 dir, std::vector<glm::vec3>& verts, float radius, int& index)
{
    float max = -FLT_MAX;
    index = 0;
    for (int i = 0; i < verts.size(); i++)
    {
        float dot = glm::dot(verts[i], dir);
        if (dot > max)
        {
            max = dot;
            index = i;
        }
    }
    return verts[index] + glm::normalize(dir) * radius;
}

GJKEvolution PhysicsSystem::AddSupportToSimplex(std::vector<glm::vec3>& simp, glm::vec3 vert, glm::vec3 dir)
{
    simp.push_back(vert);
    return glm::dot(dir, vert) >= 0 ? GJKEvolution::evolving : GJKEvolution::notIntersecting;
}

GJKEvolution PhysicsSystem::EvolveSimplex(std::vector<glm::vec3>& simp, glm::vec3& direction)
{
    switch (simp.size())
    {
    case 2:

        // line ab is the line formed by the first two vertices
        glm::vec3 ab = simp[1] - simp[0];
        // line a0 is the line from the first vertex to the origin
        glm::vec3 dO = simp[0] * -1.0f;

        if (glm::dot(ab, dO) > 0)
        {
            // use the triple-cross-product to calculate shapeA direction perpendicular
            // to line ab in the direction of the origin
            glm::vec3 temp = glm::cross(ab, dO);
            direction = glm::cross(temp, ab);
        }
        else
        {
            simp.erase(std::next(simp.begin(), 1));
            direction = dO;
        }

        return GJKEvolution::evolving;

    case 3:
        glm::vec3 ac = simp[2] - simp[0];
        ab = simp[1] - simp[0];
        dO = simp[0] * -1.0f;

        direction = glm::cross(ac, ab);

        if (glm::dot(ac, dO) <= 0) direction *= -1;

        break;
    case 4:
        // calculate the three edges of interest
        glm::vec3 da = simp[0] - simp[3];
        glm::vec3 db = simp[1] - simp[3];
        glm::vec3 dc = simp[2] - simp[3];

        // and the direction to the origin
        dO = simp[3] * -1.0f;

        // check triangles a-b-d, b-c-d, and c-a-d
        glm::vec3 abdNorm = glm::cross(da, db);
        glm::vec3 bcdNorm = glm::cross(db, dc);
        glm::vec3 cadNorm = glm::cross(dc, da);

        if (glm::dot(bcdNorm, dO) > 0)
        {
            simp.erase(simp.begin());
            direction = bcdNorm;
        }
        else if (glm::dot(abdNorm, dO) > 0)
        {
            simp.erase(std::next(simp.begin(), 2));
            direction = abdNorm;
        }
        else if (glm::dot(cadNorm, dO) > 0)
        {
            simp.erase(std::next(simp.begin(), 1));
            direction = cadNorm;
        }
        else
        {
            // the origin is inside all of the triangles!
            return GJKEvolution::intersecting;
        }
        break;

    default:
        return GJKEvolution::notIntersecting;
    }
    return GJKEvolution::evolving;
}

void PhysicsSystem::EPA(std::vector<glm::vec3>& simp, CollisionPacket& collision, 
    std::vector<glm::vec3>& aVerts, float aRadius, glm::mat4 aTransform, 
    std::vector<glm::vec3>& bVerts, float bRadius, glm::mat4 bTransform)
{
    std::vector<glm::vec3> polytope = std::vector<glm::vec3>(simp);
    std::vector<int> faces = 
    {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    int minFace = 0;
    std::vector<glm::vec4> normals = GetFaceNormals(polytope, faces, minFace);

    glm::vec3 minNormal = { 0,1,0 };
    float minDistance = FLT_MAX;
    while (minDistance == FLT_MAX)
    {
        minNormal = glm::vec3(normals[minFace].x, normals[minFace].y, normals[minFace].z);
        minDistance = normals[minFace].w;

        glm::vec3 support = CalculateSupport(aVerts, aRadius, bVerts, bRadius, minNormal);
        float sDistance = glm::dot(minNormal, support);

        if (abs(sDistance - minDistance) > 0.001f)
        {
            minDistance = FLT_MAX;
            std::vector<std::pair<int, int>> uniqueEdges;
            for (int i = 0; i < normals.size(); i++)
            {
                if (glm::dot(glm::vec3(normals[i]), support) > glm::dot(glm::vec3(normals[i]), polytope[faces[i * 3]]))
                {
                    int f = i * 3;
                    AddIfUniqueEdge(uniqueEdges, faces, f + 0, f + 1);
                    AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                    AddIfUniqueEdge(uniqueEdges, faces, f + 2, f + 0);

                    faces[f + 2] = faces[faces.size() - 1]; faces.pop_back();
                    faces[f + 1] = faces[faces.size() - 1]; faces.pop_back();
                    faces[f + 0] = faces[faces.size() - 1]; faces.pop_back();

                    normals[i] = normals[normals.size() - 1];
                    normals.pop_back();
                    i--;
                }
            }

            std::vector<int> newFaces;
            for(int edge = 0; edge < uniqueEdges.size(); edge++)
            {
                newFaces.push_back(uniqueEdges[edge].first);
                newFaces.push_back(uniqueEdges[edge].second);
                newFaces.push_back(polytope.size());
            }

            polytope.push_back(support);

            int newMinFace = 0;
            std::vector<glm::vec4> newNormals = GetFaceNormals(polytope, newFaces, newMinFace);
            float oldMinDistance = FLT_MAX;
            for (int i = 0; i < normals.size(); i++)
            {
                if (normals[i].w < oldMinDistance)
                {
                    oldMinDistance = normals[i].w;
                    minFace = i;
                }
            }

            float newMinDistance = newNormals[newMinFace].w;

            if (newMinDistance < oldMinDistance)
            {
                minFace = newMinFace + normals.size();
            }

            for(int edge = 0; edge < newFaces.size(); edge++)
            {
                faces.push_back(newFaces[edge]);
            }

            for(int norm = 0; norm < newNormals.size(); norm++)
            {
                normals.push_back(newNormals[norm]);
            }
        }
    }

    collision.normal = glm::vec3(minNormal.x, minNormal.y, minNormal.z);
    collision.depth = minDistance;
}

std::vector<glm::vec4> PhysicsSystem::GetFaceNormals(std::vector<glm::vec3>& polytope, std::vector<int> faces, int& face)
{
    std::vector<glm::vec4> normals;

    int minTriangle = 0;
    float minDistance = FLT_MAX;
    for (int i = 0; i < faces.size(); i += 3)
    {
        glm::vec3 a = polytope[faces[i]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        float distance = glm::dot(normal, a);

        if (distance < 0)
        {
            normal *= -1.0f;
            distance *= -1.0f;
        }
        glm::vec4 packet = glm::vec4(normal.x, normal.y, normal.z, distance);
        normals.push_back(packet);

        if (distance < minDistance)
        {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }
    face = minTriangle;
    return normals;
}

void PhysicsSystem::AddIfUniqueEdge(std::vector<std::pair<int, int>>& uniqueEdges, std::vector<int> edge, int aVert, int bVert)
{
    //std::find(verts.begin(), verts.end(), currIndex)
    std::vector<std::pair<int, int>>::iterator edgePosition = std::find(uniqueEdges.begin(), uniqueEdges.end(), std::pair<int,int>(edge[bVert], edge[aVert]));
    if (edgePosition != uniqueEdges.end() && edgePosition->second == edge[aVert])
    {
        uniqueEdges.erase(edgePosition);
    }
    else
    {
        uniqueEdges.push_back(std::pair<int, int>(edge[aVert], edge[bVert]));
    }
}

void PhysicsSystem::CalculateCollsionPoint(std::vector<glm::vec3>& aVerts, Shape& aShape, std::vector<glm::vec3>& bVerts, Shape& bShape, glm::vec3 normal, glm::vec3& contactPoint)
{
    contactPoint = {0,0,0};

    int aMostIndex, bMostIndex;

    glm::vec3 aMost = SupportFunction(-normal, aVerts, aShape.radius, aMostIndex);
    glm::vec3 bMost = SupportFunction(normal, bVerts, bShape.radius, bMostIndex);

    if (aVerts.size() == 1)
    {
        contactPoint = aMost;
        return;
    }
    else if (bVerts.size() == 1)
    {
        contactPoint = bMost;
        return;
    }

    std::vector<int> aFaceVertices; aFaceVertices.push_back(aMostIndex);
    std::vector<int> bFaceVertices; bFaceVertices.push_back(bMostIndex);

    for (int i = 0; i < aShape.vertices[aMostIndex].edges.size(); i++)
    {
        AddFaceVert(-normal, aMostIndex, aShape.vertices[aMostIndex].edges[i], aVerts, aShape, aFaceVertices);
    }

    for (int i = 0; i < bShape.vertices[bMostIndex].edges.size(); i++)
    {
        AddFaceVert(normal, bMostIndex, bShape.vertices[bMostIndex].edges[i], bVerts, bShape, bFaceVertices);
    }

    if (aFaceVertices.size() == 1) { contactPoint = aMost; return; }
    if (bFaceVertices.size() == 1) { contactPoint = bMost; return; }

    glm::vec3 colUp = glm::normalize(bVerts[bFaceVertices[0]] - bVerts[bFaceVertices[1]]);
    glm::vec3 colRight = glm::normalize(glm::cross(colUp, normal));
    colUp = glm::normalize(glm::cross(normal, colRight));

    std::vector<glm::vec2> a2D; a2D.reserve(aFaceVertices.size());
    std::vector<glm::vec2> b2D; b2D.reserve(bFaceVertices.size());


    for (int i = 0; i < a2D.size(); i++)
    {
        float x = glm::dot(aVerts[aFaceVertices[i]], colRight);
        float y = glm::dot(aVerts[aFaceVertices[i]], colUp);
        a2D[i] = glm::vec2(x, y);
    }

    for (int i = 0; i < b2D.size(); i++)
    {
        float x = glm::dot(bVerts[bFaceVertices[i]], colRight);
        float y = glm::dot(bVerts[bFaceVertices[i]], colUp);

        b2D[i] = glm::vec2(x, y);
    }

    std::vector<glm::vec2> contactPoints;

    for (int i = 0; i < a2D.size(); i++)
    {
        glm::vec2 a = a2D[i];
        glm::vec2 b = a2D[(i + 1) % a2D.size()] - a;

        for (int j = 0; j < b2D.size(); j++)
        {
            glm::vec2 c = b2D[j];
            glm::vec2 d = b2D[(j + 1) % b2D.size()] - c;

            float denominator = d.y * b.x - b.y * d.x;
            if (denominator != 0)
            {
                float numerator = a.y * b.x - b.y * a.x - c.y * b.x + b.y * c.x;
                float t2 = numerator / denominator;
                if (t2 >= 0 && t2 <= 1)
                {
                    float t1 = -1;
                    if (abs(b.x) > abs(b.y))
                    {
                        t1 = c.x + d.x * t2 - a.x;
                        t1 /= b.x;
                    }
                    else if (b.y != 0)
                    {
                        t1 = c.y + d.y * t2 - a.y;
                        t1 /= b.y;
                    }
                    if (t1 >= 0 && t1 <= 1)
                    {
                        glm::vec2 contact = c + d * t2;
                        contactPoints.push_back(contact);
                    }
                }
            }
        }
    }

    if (contactPoints.size() > 0)
    {
        contactPoint = {0,0,0};
        for(int vert = 0; vert < contactPoints.size(); vert++)
        {
            contactPoint += contactPoints[vert].x * colRight + contactPoints[vert].y * colUp;
        }
        contactPoint /= contactPoints.size();
        contactPoint += normal * (glm::dot(normal, aMost) + glm::dot(normal, bMost)) / 2.0f;
    }
    else
    {
        glm::vec2 aApproxCentre = a2D[0];
        glm::vec2 bApproxCentre = {0,0};

        for (int i = 0; i < b2D.size(); i++)
        {
            bApproxCentre += b2D[i];
        }
        glm::vec2 contactPoint2D;

        glm::vec2 planeNormal = b2D[1] - b2D[0];
        planeNormal = glm::vec2(planeNormal.y, -planeNormal.x);
        float planeDisplacement = glm::dot(b2D[1], planeNormal);
        float depthSign = glm::dot(a2D[0], planeNormal) - planeDisplacement;

        for (int i = 1; i < a2D.size(); i++)
        {
            if (signbit(depthSign) != signbit(glm::dot(a2D[i], planeNormal) - planeDisplacement))
            {
                contactPoint2D = bApproxCentre / (float)b2D.size();
                contactPoint = contactPoint2D.x * colRight + contactPoint2D.y * colUp;
                contactPoint += normal * (glm::dot(normal, aMost) + glm::dot(normal, bMost)) / 2.0f;
                return;
            }
            aApproxCentre += a2D[i];
        }

        contactPoint2D = aApproxCentre / (float)a2D.size();
        contactPoint = contactPoint2D.x * colRight + contactPoint2D.y * colUp;
        contactPoint += normal * (glm::dot(normal, aMost) + glm::dot(normal, bMost)) / 2.0f;
        return;
    }
}

void PhysicsSystem::AddFaceVert(glm::vec3 normal, int originIndex, int currIndex, std::vector<glm::vec3>& worldSpace, Shape shape, std::vector<int> verts)
{
    if (glm::dot(normal, (worldSpace[currIndex] - worldSpace[originIndex])) > COLLISION_FACE_THRESHOLD)
    {
        if (std::find(verts.begin(), verts.end(), currIndex) == verts.end())
        {
            verts.push_back(currIndex);
            for (int i = 0; i < shape.vertices[currIndex].edges.size(); i++)
            {
                AddFaceVert(normal, originIndex, shape.vertices[currIndex].edges[i], worldSpace, shape, verts);
            }
        }
    }
}

void PhysicsSystem::Resolution(CollisionPacket& collision)
{
    if(collision.depth < 0) return;
    
    collision.normal = glm::normalize(collision.normal * glm::dot(collision.aPos - collision.bPos, collision.normal));

    //Calculate R for each shape
    glm::vec3 rA = collision.worldContact - collision.aPos;
    glm::vec3 rB = collision.worldContact - collision.bPos;

    float totalInverseMass = (collision.objectA->invMass + collision.objectB->invMass);

    // Relative Vel = Liner Vel and Angular Vel at point of Collision
    glm::vec3 relativeVelocity =
        (collision.objectA->velocity + glm::cross(collision.objectA->angularVelocity, rA))
        - (collision.objectB->velocity + glm::cross(collision.objectB->angularVelocity, rB));

    //Calculate Average Elasticity
    float elasticCoef = collision.objectA->elasticCoef + collision.objectB->elasticCoef;
    elasticCoef /= 2;


    glm::vec3 aDenomComponentVector = collision.objectA->invWorldIT * glm::cross(rA, collision.normal);
    glm::vec3 bDenomComponentVector = collision.objectB->invWorldIT * glm::cross(rB, collision.normal);

    float aDenomComponentFloat = glm::dot(glm::cross(aDenomComponentVector, rA), collision.normal);
    float bDenomComponentFloat = glm::dot(glm::cross(bDenomComponentVector, rB), collision.normal);

    float denom = aDenomComponentFloat + bDenomComponentFloat;

    float j = -(1 + elasticCoef) * glm::dot(relativeVelocity, collision.normal) /
        (totalInverseMass + denom);

    if (j <= 0) return;

    //Add depenertration to collidable, ensures too much depenertration per frame doesnt occure
    collision.objectA->netDepen = AddDepen(collision.normal * (collision.depth) * collision.objectA->invMass / totalInverseMass, collision.objectA->netDepen);
    collision.objectB->netDepen = AddDepen(-collision.normal * (collision.depth) * collision.objectB->invMass / totalInverseMass, collision.objectB->netDepen);


    glm::vec3 linearRestitution = j * collision.normal;

    collision.objectA->velocity += linearRestitution * collision.objectA->invMass;
    collision.objectB->velocity -= linearRestitution * collision.objectB->invMass;

    glm::vec3 angularRestitutionA = glm::cross(rA, linearRestitution);
    glm::vec3 angularRestitutionB = glm::cross(rB, linearRestitution);

    collision.objectA->angularMomentum += angularRestitutionA;
    collision.objectB->angularMomentum -= angularRestitutionB;
}

glm::vec3 PhysicsSystem::AddDepen(glm::vec3 newDepen, glm::vec3 currDepen)
{
    glm::vec3 nextDepen;
    if (glm::length(newDepen) <= 0.0000001f)
    {
        return currDepen;
    }
    if (glm::dot(newDepen, currDepen) <= 0.0000001f)
    {
        nextDepen = newDepen + currDepen;
    }
    else
    {
        glm::vec3 normalNet = glm::normalize(currDepen);
        float amountAlreadyDepened = glm::dot(normalNet, newDepen);
        glm::vec3 changeInNet = currDepen - amountAlreadyDepened * normalNet;

        if (glm::dot(changeInNet, currDepen) < 0)
        {
            nextDepen = newDepen;
        }
        else
        {
            nextDepen = changeInNet + newDepen;
        }
    }
    return nextDepen;
}

void PhysicsSystem::Integration(PhysicsComponent& body, TransformComponent& transform, float deltaTime)
{
    if (body.isGravitated && body.invMass != 0)
    {
        body.force += gravity / body.invMass;
    }

    body.velocity *= 1 - 0.0001f * body.drag;
    body.velocity += deltaTime * body.invMass * body.force;
    body.angularMomentum *= 1 - 0.01f * body.angularDrag;
    body.angularMomentum += deltaTime * body.torque;

    body.angularVelocity = glm::transpose(glm::mat3(transform.rotation)) * body.angularMomentum;
    body.angularVelocity = body.invBodyIT *  body.angularVelocity;
    body.angularVelocity = glm::mat3(transform.rotation) * body.angularVelocity;

    transform.position += body.netDepen;
    body.netDepen = {0,0,0};

    transform.position += deltaTime * body.velocity;

    //Cross Product Matrix
    glm::mat3 angVel = {
        0, body.angularVelocity.z, -body.angularVelocity.y,
        -body.angularVelocity.z, 0, body.angularVelocity.x,
        body.angularVelocity.y, -body.angularVelocity.x, 0
    };

    angVel *= deltaTime;

    glm::mat3 rotation = angVel * glm::mat3(transform.rotation);
    rotation += glm::mat3(transform.rotation);
    transform.rotation = OrthonormalizeOrientation(rotation);

    body.invWorldIT = glm::mat3(transform.rotation) * body.invBodyIT;
    body.invWorldIT = body.invWorldIT * glm::transpose(body.invBodyIT);


    body.torque = { 0,0,0 };
    body.force = { 0,0,0 };
}

glm::quat PhysicsSystem::OrthonormalizeOrientation(const glm::mat4 rotation)
{
    glm::vec3 X = {rotation[0][0], rotation[0][1], rotation[0][2]};
    glm::vec3 Y = {rotation[1][0], rotation[1][1], rotation[1][2]};
    glm::vec3 Z;

    Z = glm::cross(X, Y);
    Y = glm::cross(Z, X);
    X = glm::normalize(X);
    Y = glm::normalize(Y);
    Z = glm::normalize(Z);

    glm::mat3 orientation{
        X.x, X.y, X.z,
        Y.x, Y.y, Y.z,
        Z.x, Z.y, Z.z
    };

    return glm::quat(orientation);
}
