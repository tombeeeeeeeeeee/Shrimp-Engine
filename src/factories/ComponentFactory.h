#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"
#include "../factories/AssetFactory.h"
/// <summary>
/// Component Factory makes and alters Components
/// </summary>
class ComponentFactory
{
public:

	ComponentFactory(
		std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents,
		std::unordered_map<unsigned int, RenderComponent*>& renderComponents,
		std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
		AssetFactory& _assFact
	);
	~ComponentFactory();
	ComponentFactory(ComponentFactory& compFact) :
		entityCount(compFact.entityCount),
		physicsComponents(compFact.physicsComponents),
		transformComponents(compFact.transformComponents),
		renderComponents(compFact.renderComponents),
		assFact(compFact.assFact) {}

	/// <summary>
	/// Makes entity and adds to the entity count.
	/// </summary>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeEntity() { return entityCount++; }

	/// <summary>
	/// Creates an entity with a transform and camera component
	/// </summary>
	/// <param name="position"> transform position</param>
	/// <param name="eulers"> camera position </param>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeCamera(vec3 position, vec3 eulers);

	/// <summary>
	/// Makes a transform component with a position at 0,0,0
	/// </summary>
	/// <returns>unsigned int transform component entity id</returns>
	unsigned int MakeEmptyTransform();

	/// <summary>
	/// Makes a transform component
	/// </summary>
	/// <param name="position">position of component</param>
	/// <param name="eulers">euler of component</param>
	/// <returns>unsigned int transform component entity id</returns>
	unsigned int MakeEmptyTransform(vec3 position, vec3 eulers);

	/// <summary>
	/// Makes a rat with a render component and transform component
	/// </summary>
	/// <param name="position">position of component</param>
	/// <param name="eulers">euler of component</param>
	/// <returns>unsigned int rat entity id</returns>
	unsigned int MakeRat(vec3 position, vec3 eulers);

	/// <summary>
	/// Makes a cube with a render component and transform component
	/// </summary>
	/// <param name="position">position of component</param>
	/// <param name="eulers">euler of component</param>
	/// <returns>unsigned int cube entity id</returns>
	unsigned int MakeCube(vec3 position, vec3 eulers);

	/// <summary>
	/// Add a new render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns> new render component</returns>
	RenderComponent* AddRenderComponent(unsigned int _entity);

	/// <summary>
	/// Add a provided render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="rend"> render component to add to entity/param>
	/// <returns> render component</returns>
	RenderComponent* AddRenderComponent(unsigned int _entity, RenderComponent* rend);
	PhysicsComponent* AddPhysicsComponent(unsigned int _entity);

	unsigned int MakeSkyBox(std::string textureFileNames[6]);

private:
	/// <summary>
	/// Starting entity count value (must be above 0)
	/// </summary>
	unsigned int entityCount = 1;

	/// <summary>
	/// Physics Components in scene
	/// </summary>
	std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents;

	/// <summary>
	/// Render Components in scene
	/// </summary>
	std::unordered_map<unsigned int, RenderComponent*>& renderComponents;

	/// <summary>
	/// Transform Components in scene
	/// </summary>
	std::unordered_map<unsigned int, TransformComponent*>& transformComponents;

	/// <summary>
	/// Asset Factory for asset making
	/// </summary>
	AssetFactory& assFact;
};

