#include "LinearAlgebra.h"

mat4 createTranslationMatrix(vec3 translation)
{
	return mat4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		translation.x, translation.y, translation.z, 1
	);
}

mat4 rotationXAxisMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[5] = c; rot.entries[9] = -s;
	rot.entries[6] = s; rot.entries[10] = c;
	return rot;
}

mat4 rotationYAxisMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[0] = c; rot.entries[2] = -s;
	rot.entries[8] = s; rot.entries[10] = c;
	return rot;
}

mat4 rotationZAxisMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[0] = c; rot.entries[4] = -s;
	rot.entries[1] = s; rot.entries[5] = c;
	return rot;
}
