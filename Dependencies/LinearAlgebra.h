#pragma once
#include "../Utilities.h"

/// <summary>
/// PI to 6 digits (Used in LinerAlgebra.h)
/// </summary>
const float LA_PI = 3.141596f;

struct vec4
{
	float x;
	float y;
	float z;
	float w;

	vec4()
	{
		x = 0; y = 0; z = 0; w = 0;
	}

	vec4(float _x, float _y, float _z, float _w)
	{
		x = _x; y = _y; z = _z; w = _w;
	}

	float magnitude{ sqrtf(x * x + y * y + z * z + w * w) };
	 
	/// <summary>
	/// Returns the normal of the vector
	/// </summary>
	vec4 normal();


	/// <summary>
	/// Returns the dot product of two vector4s
	/// </summary>
	float DotProduct(vec4 b)
	{
		return this->x * b.x + this->y * b.y + this->z * b.z + this->w * b.w;
	}

	vec4 operator+(const vec4& b)
	{
		return vec4(this->x + b.x, this->y + b.y, this->z + b.z, this->w + b.w);
	}

	vec4 operator-(const vec4& b)
	{
		return vec4(this->x - b.x, this->y - b.y, this->z - b.z, this->w - b.w);
	}
};

struct vec3
{
	float x;
	float y;
	float z;

	vec3()
	{
		x = 0; y = 0; z = 0;
	}

	vec3(float _x, float _y, float _z)
	{
		x = _x; y = _y; z = _z;
	}

	float magnitude() { return sqrtf(x * x + y * y + z * z); }


	/// <summary>
	/// Returns the normal of the vector
	/// </summary>
	vec3 normal()
	{ 
		float mag = magnitude(); 
		if (mag == 0)  return vec3(0, 0, 0);
		return vec3(x / mag, y / mag, z / mag);
	}
	
	/// <summary>
	/// Returns the dot product of two vector3s
	/// </summary>
	float dot(vec3 b)
	{
		return x * b.x + y * b.y + z * b.z;
	}

	/// <summary>
	/// Returns the cross product of two vectors
	/// </summary>
	vec3 cross(vec3 b);

	vec3 operator+(const vec3& b)
	{
		return vec3(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	vec3 operator-(const vec3& b)
	{
		return vec3(this->x - b.x, this->y - b.y, this->z - b.z);
	}
};

struct mat4
{
	//Column Major
	float entries[16];

	mat4()
	{
		entries[0] = 1;  entries[1] = 0;  entries[2] = 0;  entries[3] = 0;
		entries[4] = 0;	 entries[5] = 1;  entries[6] = 0;  entries[7] = 0;
		entries[8] = 0;	 entries[9] = 0;  entries[10] = 1; entries[11] = 0;
		entries[12] = 0; entries[13] = 0; entries[14] = 0; entries[15] = 1;
	}

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

	vec3 right();

	vec3 up();

	vec3 forward();

	vec3 position();

	mat4 inverse();

	mat4 LookAtFrom(vec3 from, vec3 to);


	mat4 operator*(const mat4& m)
	{
		mat4 mat;

		//Colomn 1
		mat.entries[0] = this->entries[0] * m.entries[0] + this->entries[4] * m.entries[1] + this->entries[8] * m.entries[2] + this->entries[12] * m.entries[3];
		mat.entries[1] = this->entries[1] * m.entries[0] + this->entries[5] * m.entries[1] + this->entries[9] * m.entries[2] + this->entries[13] * m.entries[3];
		mat.entries[2] = this->entries[2] * m.entries[0] + this->entries[6] * m.entries[1] + this->entries[10] * m.entries[2] + this->entries[14] * m.entries[3];
		mat.entries[3] = this->entries[3] * m.entries[0] + this->entries[7] * m.entries[1] + this->entries[11] * m.entries[2] + this->entries[15] * m.entries[3];
		//Colomn 2
		mat.entries[4] = this->entries[0] * m.entries[4] + this->entries[4] * m.entries[5] + this->entries[8] * m.entries[6] + this->entries[12] * m.entries[7];
		mat.entries[5] = this->entries[1] * m.entries[4] + this->entries[5] * m.entries[5] + this->entries[9] * m.entries[6] + this->entries[13] * m.entries[7];
		mat.entries[6] = this->entries[2] * m.entries[4] + this->entries[6] * m.entries[5] + this->entries[10] * m.entries[6] + this->entries[14] * m.entries[7];
		mat.entries[7] = this->entries[3] * m.entries[4] + this->entries[7] * m.entries[5] + this->entries[11] * m.entries[6] + this->entries[15] * m.entries[7];
		//Colomn 3
		mat.entries[8] = this->entries[0] * m.entries[8] + this->entries[4] * m.entries[9] + this->entries[8] * m.entries[10] + this->entries[12] * m.entries[11];
		mat.entries[9] = this->entries[1] * m.entries[8] + this->entries[5] * m.entries[9] + this->entries[9] * m.entries[10] + this->entries[13] * m.entries[11];
		mat.entries[10] = this->entries[2] * m.entries[8] + this->entries[6] * m.entries[9] + this->entries[10] * m.entries[10] + this->entries[14] * m.entries[11];
		mat.entries[11] = this->entries[3] * m.entries[8] + this->entries[7] * m.entries[9] + this->entries[11] * m.entries[10] + this->entries[15] * m.entries[11];
		//Colomn 4
		mat.entries[12] = this->entries[0] * m.entries[12] + this->entries[4] * m.entries[13] + this->entries[8] * m.entries[14] + this->entries[12] * m.entries[15];
		mat.entries[13] = this->entries[1] * m.entries[12] + this->entries[5] * m.entries[13] + this->entries[9] * m.entries[14] + this->entries[13] * m.entries[15];
		mat.entries[14] = this->entries[2] * m.entries[12] + this->entries[6] * m.entries[13] + this->entries[10] * m.entries[14] + this->entries[14] * m.entries[15];
		mat.entries[15] = this->entries[3] * m.entries[12] + this->entries[7] * m.entries[13] + this->entries[11] * m.entries[14] + this->entries[15] * m.entries[15];

		return mat;
	}

	friend ostream& operator<<(ostream& os, mat4 const& mat);
};

mat4 createTranslationMatrix(vec3 translation);
mat4 rotationXAxisMatrix(float angle, bool degrees = true);
mat4 rotationYAxisMatrix(float angle, bool degrees = true);
mat4 rotationZAxisMatrix(float angle, bool degrees = true);

mat4 ViewMatrix(vec3 from, vec3 to);
mat4 ProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane, bool fovDegrees = true);

vec4 Normalise(vec4 vec);
vec3 Normalise(vec3 vec);

vec3 cross(vec3 a, vec3 b);