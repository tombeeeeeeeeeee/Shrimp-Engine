#include "HierarchySystem.h"

void HierarchySystem::Update()
{
	std::unordered_map<unsigned int, TransformComponent*>::iterator iter;
	
	for(iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
		updated[iter->first] = false;

	for (iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
			GlobalTransformUpdate(iter->first, transformComponents);

	updated.clear();
}

void HierarchySystem::GlobalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent*>& transformComponents)
{
	if (!updated[entity])
	{
		updated[entity] = true;

		TransformComponent* tc = transformComponents[entity];

		if (tc->parent > 0)
			tc->globalTransform = transformComponents[tc->parent]->globalTransform * tc->localTransform;
		else
			tc->globalTransform = tc->localTransform;

		std::vector<unsigned int>::iterator iter;
		for (iter = tc->children.begin(); iter != tc->children.end(); iter++)
			GlobalTransformUpdate(*iter, transformComponents);
	}
}

void HierarchySystem::SetParent(unsigned int child, unsigned int parent)
{
	bool realParent = transformComponents.find(parent) != transformComponents.end();
	if ((realParent || parent == 0) && transformComponents.find(child) != transformComponents.end())
	{
		transformComponents[child]->parent = parent;
		if (realParent)
			transformComponents[parent]->children.push_back(child);
	}
	
}
