#pragma once
#include "config.h"

namespace LGP
{
	struct TransformComponent
	{
		glm::mat4 globalTransform = glm::identity<glm::mat4>();

		glm::vec3 position = { 0,0,0 };
		glm::vec3 scale = { 1,1,1 };
		glm::quat rotation = glm::identity<glm::quat>();

		unsigned int parent = 0;
		std::vector<unsigned int> children;
	};
}