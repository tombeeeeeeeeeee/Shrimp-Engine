#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"

class ComponentFactory
{
public:

	ComponentFactory(
		std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
		std::unordered_map<unsigned int, RenderComponent>& renderComponents,
		std::unordered_map<unsigned int, TransformComponent>& transformComponents
	);

	~ComponentFactory();

	unsigned int MakeEntity() { return entityCount++; }

	unsigned int MakeCamera(vec3 position, vec3 eulers);

	unsigned int MakeEmptyTransform();
	unsigned int MakeEmptyTransform(vec3 position, vec3 eulers);

	unsigned int MakeRat(vec3 position, vec3 eulers);
	unsigned int MakeCube(vec3 position, vec3 eulers, vec3 eulerVelocity);

	void AddRenderComponent(unsigned int _entity);
	void AddPhysicsComponent(unsigned int _entity);
	void AddCameraComponent(unsigned int _enity);

private:
	unsigned int entityCount = 0;

	std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents;
	std::unordered_map<unsigned int, RenderComponent>& renderComponents;
	std::unordered_map<unsigned int, TransformComponent>& transformComponents;


};

