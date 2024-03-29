#include "TransformComponent.h"



vec3 TransformComponent::position()
{
	return { globalTransform.entries[12], globalTransform.entries[13] , globalTransform.entries[14] };
}

vec3 TransformComponent::localPosition()
{
	return { localTransform.entries[12], localTransform.entries[13] , localTransform.entries[14] };
}

vec3 TransformComponent::eulers(bool radians)
{
	float eulX = 0;
	float eulY = 0;
	float eulZ = 0;

	float sy = sqrt(localTransform.entries[0] * localTransform.entries[0] + localTransform.entries[1] * localTransform.entries[1]);

	bool singular = sy < 1e-6; // If

	float x, y, z;
	if (!singular)
	{
		x = atan2(localTransform.entries[6], localTransform.entries[10]);
		y = atan2(-localTransform.entries[2], sy);
		z = atan2(localTransform.entries[1], localTransform.entries[0]);
	}
	else
	{
		x = atan2(-localTransform.entries[9], localTransform.entries[5]);
		y = atan2(-localTransform.entries[2], sy);
		z = 0;
	}
	return vec3(x, y, z);

	if (abs(localTransform.entries[2]) != 1)
	{
		eulY = asinf(localTransform.entries[2]);
		float c = cos(eulY);
		eulX = atan2f(localTransform.entries[6] / c, localTransform.entries[10] / c);
		eulZ = atan2f(localTransform.entries[1] / c, localTransform.entries[0] / c);
	}
	else
	{
		eulZ = 0;
		eulY = LA_PI / 2.0f;
		if (globalTransform.entries[2] == -1)
		{
			eulX = atan2f(localTransform.entries[4], localTransform.entries[8]);
		}
		else
		{
			eulY *= -1;
			eulX = atan2f(-localTransform.entries[4], -localTransform.entries[8]);
		}
	}
	vec3 eul = { eulX, eulY, eulZ };
	if(!radians) eul *= 180 / LA_PI;
	return eul; 
}

vec3 TransformComponent::eulersGlobal(bool radians)
{
	float eulX = 0;
	float eulY = 0;
	float eulZ = 0;

	float sy = sqrt(globalTransform.entries[0] * globalTransform.entries[0] + globalTransform.entries[1] * globalTransform.entries[1]);

	bool singular = sy < 1e-6; // If

	float x, y, z;
	if (!singular)
	{
		x = atan2(globalTransform.entries[6], globalTransform.entries[10]);
		y = atan2(-globalTransform.entries[2], sy);
		z = atan2(globalTransform.entries[1], globalTransform.entries[0]);
	}
	else
	{
		x = atan2(-globalTransform.entries[9], globalTransform.entries[5]);
		y = atan2(-globalTransform.entries[2], sy);
		z = 0;
	}
	return vec3(x, y, z);

	if (abs(globalTransform.entries[2]) != 1)
	{
		eulY = asinf(globalTransform.entries[2]);
		float c = cos(eulY);
		eulX = atan2f(globalTransform.entries[6] / c, globalTransform.entries[10] / c);
		eulZ = atan2f(globalTransform.entries[1] / c, globalTransform.entries[0] / c);
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
	if (!radians) eul *= 180 / LA_PI;
	return eul;
}
