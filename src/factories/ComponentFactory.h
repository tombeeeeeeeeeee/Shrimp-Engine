#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"
#include "../factories/AssetFactory.h"

class ComponentFactory
{
public:

	ComponentFactory(
		std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
		std::unordered_map<unsigned int, RenderComponent>& renderComponents,
		std::unordered_map<unsigned int, TransformComponent>& transformComponents,
		AssetFactory& _assFact
	);

	~ComponentFactory();

	unsigned int MakeEntity() { return entityCount++; }

	unsigned int MakeCamera(vec3 position, vec3 eulers);

	unsigned int MakeEmptyTransform();
	unsigned int MakeEmptyTransform(vec3 position, vec3 eulers);

	unsigned int MakeRat(vec3 position, vec3 eulers);
	unsigned int MakeCube(vec3 position, vec3 eulers);

	RenderComponent* AddRenderComponent(unsigned int _entity);
	PhysicsComponent* AddPhysicsComponent(unsigned int _entity);

private:
	unsigned int entityCount = 1;

	std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents;
	std::unordered_map<unsigned int, RenderComponent>& renderComponents;
	std::unordered_map<unsigned int, TransformComponent>& transformComponents;

	AssetFactory& assFact;
};

