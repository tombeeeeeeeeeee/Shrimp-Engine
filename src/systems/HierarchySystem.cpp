#include "HierarchySystem.h"

void HierarchySystem::Update()
{
	std::unordered_map<unsigned int, TransformComponent>::iterator iter;
	
	//Reset update list
	for(iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
		updated[iter->first] = false;

	//Begin iteration
	for (iter = transformComponents.begin(); iter != transformComponents.end(); iter++)
			GlobalTransformUpdate(iter->first, transformComponents);

	//Clear update
	updated.clear();
}

void HierarchySystem::GlobalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent>& transformComponents)
{
	if (!updated[entity])
	{
		updated[entity] = true;

		TransformComponent* tc = &transformComponents[entity];

		//Check if the entity has a parent
		if (tc->parent > 0)
			tc->globalTransform = transformComponents[tc->parent].globalTransform * tc->LocalTransform();
		else
			tc->globalTransform = tc->LocalTransform;

		//Send recursion to children.
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
		transformComponents[child].parent = parent;
		if (realParent)
			transformComponents[parent].children.push_back(child);
	}
	
}
