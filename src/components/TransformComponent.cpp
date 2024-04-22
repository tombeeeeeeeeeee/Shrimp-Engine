#include "TransformComponent.h"



vec3 TransformComponent::Position()
{
	return { globalTransform.entries[12], globalTransform.entries[13] , globalTransform.entries[14] };
}

vec3 TransformComponent::LocalPosition()
{
	return position;
}

vec3 TransformComponent::LocalEulers(bool radians)
{
	return radians ? eulers : eulers * (180.0f / LA_PI);
}

mat4 TransformComponent::LocalTransform()
{
	return TranslationMatrix(position) 
			* RotationXMatrix(eulers.x) 
			* RotationXMatrix(eulers.y) 
			* RotationXMatrix(eulers.z) 
			* ScaleMatrix(scale);
}

vec3 TransformComponent::Eulers(bool radians)
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
