#pragma once
#include <config.h>
#include "../components/TransformComponent.h"

class HierarchySystem
{
public: 
	HierarchySystem(std::unordered_map<unsigned int, TransformComponent*>& _transformComponents) : transformComponents(_transformComponents){}

	void Update();
	void GlobalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent*>& transformComponents);

	void SetParent(unsigned int child, unsigned int parent = 0);

private: 
	std::unordered_map<unsigned int, TransformComponent*>& transformComponents;
	std::unordered_map<unsigned int, bool> updated;
};

