#include "TransformComponent.h"



vec3 TransformComponent::position()
{
	return { globalTransform.entries[12], globalTransform.entries[13] , globalTransform.entries[14] };
}

vec3 TransformComponent::eulers()
{
	float eulX = 0;
	float eulY = 0;
	float eulZ = 0;

	if (abs(globalTransform.entries[2]) != 1)
	{
		float eulY = -asinf(globalTransform.entries[2]);
		float c = cos(eulY);
		float eulX = atan2f(globalTransform.entries[6] / c, globalTransform.entries[10] / c);
		float eulZ = atan2f(globalTransform.entries[0] / c, globalTransform.entries[1] / c);
	}
	else
	{
		eulZ = 0;
		eulY = LA_PI / 2.0f;
		if (globalTransform.entries[2] == -1)
		{
			eulX = atan2f(globalTransform.entries[4], globalTransform.entries[8]);
		}
		else
		{
			eulY *= -1;
			eulX = atan2f(-globalTransform.entries[4], -globalTransform.entries[8]);
		}
	}
	vec3 eul = { eulX, eulY, eulZ };
	eul *= 180 / LA_PI;
	return eul; 
}
