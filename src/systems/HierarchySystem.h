#pragma once
#include <config.h>
#include "../components/TransformComponent.h"

class HierarchySystem
{
public: 
	HierarchySystem(GLFWwindow* _window) : window(_window){}

	void Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents);
	void globalTransformUpdate(unsigned int entity, std::unordered_map<unsigned int, TransformComponent*>& transformComponents);

private: 
	GLFWwindow* window;
	std::unordered_map<unsigned int, bool> updated;
};

