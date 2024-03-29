#pragma once
#include <config.h>

struct TransformComponent
{
	mat4 globalTransform;
	mat4 localTransform;
	unsigned int parent = -1;
	std::vector<unsigned int> children;

	vec3 Position();
	vec3 LocalPosition();
	vec3 Eulers(bool radians = true);
	vec3 LocalEulers(bool radians = true);
};