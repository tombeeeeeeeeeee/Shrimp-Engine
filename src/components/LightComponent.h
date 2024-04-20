#pragma once
#include <config.h>
enum class LightType
{
	ambient = 1,
	directional = 2,
	point = 3,
	spot = 4,
	count
};

struct LightComponent
{
	LightType lightType = LightType::ambient;
	vec3 colour = { 0,0,0 };
	vec3 direction = { 1,0,0 };
	float range = 0;
	float linear = 1;
	float quad = 1;
	float angle = 0;

	void CalculateLinearQuadConstants();
};

