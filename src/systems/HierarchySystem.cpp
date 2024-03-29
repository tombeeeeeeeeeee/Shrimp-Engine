#include "HierarchySystem.h"

void HierarchySystem::Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents)
{
	std::unordered_map<unsigned int, TransformComponent*>::iterator iter;
	
	for(iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
		updated[iter->first] = false;

	for (iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
			globalTransformUpdate(iter->first, transformComponents);

	updated.clear();
}

void HierarchySystem::globalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent*>& transformComponents)
{
	if (!updated[entity])
	{
		updated[entity] = true;

		TransformComponent* tc = transformComponents[entity];

		if (tc->parent != 0)
			tc->globalTransform = transformComponents[tc->parent]->globalTransform * tc->localTransform;
		else
			tc->globalTransform = tc->localTransform;

		std::vector<unsigned int>::iterator iter;
		for (iter = tc->children.begin(); iter != tc->children.end(); iter++)
			globalTransformUpdate(*iter, transformComponents);
	}
}
