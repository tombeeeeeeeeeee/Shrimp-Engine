#pragma once
#include <config.h>
#include "../components/TransformComponent.h"

class HierarchySystem
{
public: 
	HierarchySystem(std::unordered_map<unsigned int, TransformComponent*>& _transformComponents) : transformComponents(_transformComponents){}
	
	/// <summary>
	/// Resets list of transforms that have had their global transform calculated
	/// </summary>
	void Update();

	/// <summary>
	/// Recursivly updates global transfrom from parent to child.
	/// </summary>
	/// <param name="entity"> Transform ID to update</param>
	/// <param name="transformComponents"> All transform components</param>
	void GlobalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent*>& transformComponents);

	/// <summary>
	/// Changes a transforms parent, and changes the parents listed children.
	/// </summary>
	/// <param name="child"> Transform ID of Child</param>
	/// <param name="parent"> Transform ID of Parent</param>
	void SetParent(unsigned int child, unsigned int parent = 0);

private:
	/// <summary>
	/// All transform components in the hierarchy
	/// </summary>
	std::unordered_map<unsigned int, TransformComponent*>& transformComponents;

	/// <summary>
	/// list of all transform components to check if they have a global transform.
	/// </summary>
	std::unordered_map<unsigned int, bool> updated;
};

