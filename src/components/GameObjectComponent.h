#pragma once
#include <config.h>

struct GameObjectComponent
{
	std::string name;
	unsigned int componentMask = 0;
};

enum ComponentMask
{
	Transform = 1,
	Render = 2,
	Light = 4,
	Camera = 8,
	Physics = 16,
};