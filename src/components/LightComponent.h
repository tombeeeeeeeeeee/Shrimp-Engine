#pragma once
#include <config.h>
enum class SHRIMP_LightType
{
	ambient,
	directional,
	point,

	//SPOT LIGHT TODO
	count
};

struct LightComponent
{
	SHRIMP_LightType lightType;
	vec3 colour;
	float radius;
	float angle;
};

