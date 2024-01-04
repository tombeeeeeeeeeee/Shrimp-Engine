#pragma once
#include "../Utilities.h"

/// <summary>
/// PI to 6 digits (Used in LinerAlgebra.h)
/// </summary>
const float LA_PI = 3.141596f;

struct mat4
{
	//Column Major
	float entries[16];

	mat4(
		float _m00, float _m10, float _m20, float _m30,
		float _m01, float _m11, float _m21, float _m31,
		float _m02, float _m12, float _m22, float _m32,
		float _m03, float _m13, float _m23, float _m33
	)
	{
		entries[0] = _m00;  entries[1] = _m10;  entries[2] = _m20;  entries[3] = _m30;
		entries[4] = _m01;  entries[5] = _m11;  entries[6] = _m21;  entries[7] = _m31;
		entries[8] = _m02;  entries[9] = _m12;  entries[10] = _m22; entries[11] = _m32;
		entries[12] = _m03; entries[13] = _m13; entries[14] = _m23; entries[15] = _m33;
	}

	mat4(float scale)
	{
		entries[0] = scale;
		entries[1] = scale;
		entries[2] = scale;
		entries[3] = 1;
	}

};

struct vec4
{
	float x, y, z, w;
};

struct vec3
{
	float x, y, z;
};

mat4 createTranslationMatrix(vec3 translation);
mat4 rotationXAxisMatrix(float angle, bool degrees = true);
mat4 rotationYAxisMatrix(float angle, bool degrees = true);
mat4 rotationZAxisMatrix(float angle, bool degrees = true);