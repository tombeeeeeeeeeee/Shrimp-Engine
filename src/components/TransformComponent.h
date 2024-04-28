#pragma once
#include <config.h>

struct TransformComponent
{
	mat4 globalTransform;
	
	vec3 position = { 0,0,0 };
	vec3 eulers   = { 0,0,0 };
	vec3 scale    = { 1,1,1 };

	unsigned int parent = 0;
	std::vector<unsigned int> children;
};