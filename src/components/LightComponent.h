#pragma once
#include <config.h>
enum class LightType
{
	ambient = 1,
	directional = 2,
	point = 3,

	//SPOT LIGHT TODO
	count
};

struct LightComponent
{
	LightType lightType = LightType::ambient;
	vec3 colour = { 0,0,0 };
	float radius = 0;
	float angle = 0;
};

