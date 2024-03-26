#pragma once
#include <config.h>

struct TransformComponent
{
	mat4 globalTransform;
	mat4 localTransform;
	unsigned int parent;
	std::vector<unsigned int> children;

	vec3 position();
	vec3 eulers(bool radians = true);
};