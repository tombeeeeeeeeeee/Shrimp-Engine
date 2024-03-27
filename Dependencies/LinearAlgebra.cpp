#include "LinearAlgebra.h"


vec2 vec4::xy() { return vec2(x, y); }

vec3 vec4::xyz() { return vec3(x, y, z); }

vec4 vec4::GetNormalised()
{
	float mag = magnitude();
	return vec4(x / mag, y / mag, z / mag, w / mag);
}

vec2 vec3::xy() { return vec2(x, y); }

vec3 vec3::cross(vec3 b)
{
	return vec3(
		y * b.z - b.y * z,
		z * b.x - b.z * x,
		x * b.y - b.x * y
	);
}

mat4 TranslationMatrix(vec3 translation)
{
	return mat4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		translation.x, translation.y, translation.z, 1
	);
}

mat4 RotationXMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[5] = c; rot.entries[9] = -s;
	rot.entries[6] = s; rot.entries[10] = c;
	return rot;
}

mat4 RotationYMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[0] = c; rot.entries[2] = -s;
	rot.entries[8] = s; rot.entries[10] = c;
	return rot;
}

mat4 RotationZMatrix(float angle, bool degrees)
{
	float radiansAngle = degrees ? LA_PI * angle / 180 : angle;
	mat4 rot;
	float s = sinf(radiansAngle);
	float c = cosf(radiansAngle);
	rot.entries[0] = c; rot.entries[4] = -s;
	rot.entries[1] = s; rot.entries[5] = c;
	return rot;
}

mat4 GetInverse(mat4 mat)
{
	return mat.GetInverse();
}

mat3 GetInverse(mat3 mat)
{
	return mat.GetInverse();
}

mat4 ViewMatrix(vec3 from, vec3 to, vec3 up)
{

	vec3 f = GetNormalised(to - from);
	vec3 r = GetNormalised(cross(f, up));
	vec3 u = GetNormalised(cross(r, f));

	 //Weird
	//return GetInverse(mat4(
	//	r.x, u.x, -f.x, 0,
	//	r.y, u.y, -f.y, 0,
	//	r.z, u.z, -f.z, 0,
	//	-(r.dot(from)), -(u.dot(from)), f.dot(from), 1
	//));

	return mat4(
		r.x, u.x, -f.x, 0,
		r.y, u.y, -f.y, 0,
		r.z, u.z, -f.z, 0,
		-(r.dot(from)), -(u.dot(from)), f.dot(from), 1
	);
}

std::ostream& operator<<(std::ostream& os, vec3 const& vec)
{
	return os << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }" << std::endl;
}

std::ostream& operator<<(std::ostream& os, vec2 const& vec)
{
	return os << "{ " << vec.x << ", " << vec.y << " }" << std::endl;
}

std::ostream& operator<<(std::ostream& os, mat4 const& mat)
{
	return 
	os << "| " << mat.entries[0] << ", " << mat.entries[4] << ", " << mat.entries[8]  << ", " << mat.entries[12] << " |" << std::endl
	   << "| " << mat.entries[1] << ", " << mat.entries[5] << ", " << mat.entries[9]  << ", " << mat.entries[13] << " |" << std::endl
	   << "| " << mat.entries[2] << ", " << mat.entries[6] << ", " << mat.entries[10] << ", " << mat.entries[14] << " |" << std::endl
	   << "| " << mat.entries[3] << ", " << mat.entries[7] << ", " << mat.entries[11] << ", " << mat.entries[15] << " |" << std::endl;
}

std::ostream& operator<<(std::ostream& os, mat3 const& mat)
{
	return
		os << "| " << mat.entries[0] << ", " << mat.entries[3] << ", " << mat.entries[6] << " |" << std::endl
		<< "| " << mat.entries[1] << ", " << mat.entries[4] << ", " << mat.entries[7] << " |" << std::endl
		<< "| " << mat.entries[2] << ", " << mat.entries[5] << ", " << mat.entries[8] << " |" << std::endl;
}

mat4 ProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane, bool fovDegrees)
{
	fov = fovDegrees ? fov * LA_PI / 360 : fov / 2;
	float t = 1 / tanf(fov);
	float n = -nearPlane;
	float f = -farPlane;

	return mat4(
		t/aspect, 0, 0, 0,
		0, t, 0, 0,
		0, 0, -(f+n)/(f-n), -1,
		0, 0, 2 * f * n / (f-n), 0
	);
}

vec4 GetNormalised(vec4 vec)
{
	return vec.GetNormalised();
}

vec3 GetNormalised(vec3 vec)
{
	return vec.GetNormalised();
}

mat4 SetPosition(mat4 mat, vec3 pos)
{
	mat.entries[12] = pos.x;
	mat.entries[13] = pos.y;
	mat.entries[14] = pos.z;
	return mat;
}

vec3 cross(vec3 a, vec3 b)
{
	return a.cross(b);
}

mat3 mat3::GetInverse()
{
	float det = GetDeterminate();
	if (det != 0)
	{
		return GetAdjugate() * (1 / det);
	}
	return *this;
}

mat3 mat3::GetAdjugate()
{
	return mat3(
		(entries[4] * entries[8] - entries[7] * entries[5]), -(entries[1] * entries[8] - entries[7] * entries[2]), (entries[1] * entries[5] - entries[4] * entries[2]),
		-(entries[3] * entries[8] - entries[6] * entries[5]), (entries[0] * entries[8] - entries[6] * entries[2]), -(entries[0] * entries[5] - entries[3] * entries[2]),
		(entries[3] * entries[7] - entries[6] * entries[4]), -(entries[0] * entries[7] - entries[1] * entries[6]), (entries[0] * entries[4] - entries[3] * entries[1])
	);
}

float mat3::GetDeterminate()
{
	return (entries[0] * (entries[4] * entries[8] - entries[7] * entries[5])) - (entries[3] * (entries[1] * entries[8] - entries[7] * entries[2])) + (entries[6] * (entries[1] * entries[5] - entries[4] * entries[2]));
}

mat4 mat4::GetInverse()
{
	mat3 m00 = mat3(entries[5], entries[6], entries[7], entries[9], entries[10], entries[11], entries[13], entries[14], entries[15]);
	mat3 m01 = mat3(entries[1], entries[2], entries[3], entries[9], entries[10], entries[11], entries[13], entries[14], entries[15]);
	mat3 m02 = mat3(entries[1], entries[2], entries[3], entries[5], entries[6], entries[7], entries[13], entries[14], entries[15]);
	mat3 m03 = mat3(entries[1], entries[2], entries[3], entries[5], entries[6], entries[7], entries[9], entries[10], entries[11]);

	mat3 m10 = mat3(entries[4], entries[6], entries[7], entries[8], entries[10], entries[11], entries[12], entries[14], entries[15]);
	mat3 m11 = mat3(entries[0], entries[2], entries[3], entries[8], entries[10], entries[11], entries[12], entries[14], entries[15]);
	mat3 m12 = mat3(entries[0], entries[2], entries[3], entries[4], entries[6], entries[7], entries[12], entries[14], entries[15]);
	mat3 m13 = mat3(entries[0], entries[2], entries[3], entries[4], entries[6], entries[7], entries[8], entries[10], entries[11]);

	mat3 m20 = mat3(entries[4], entries[5], entries[7], entries[8], entries[9], entries[11], entries[12], entries[13], entries[15]);
	mat3 m21 = mat3(entries[0], entries[1], entries[3], entries[8], entries[9], entries[11], entries[12], entries[13], entries[15]);
	mat3 m22 = mat3(entries[0], entries[1], entries[3], entries[4], entries[5], entries[7], entries[12], entries[13], entries[15]);
	mat3 m23 = mat3(entries[0], entries[1], entries[3], entries[4], entries[5], entries[7], entries[8], entries[9], entries[11]);

	mat3 m30 = mat3(entries[4], entries[5], entries[6], entries[8], entries[9], entries[10], entries[12], entries[13], entries[14]);
	mat3 m31 = mat3(entries[0], entries[1], entries[2], entries[8], entries[9], entries[10], entries[12], entries[13], entries[14]);
	mat3 m32 = mat3(entries[0], entries[1], entries[2], entries[4], entries[5], entries[6], entries[12], entries[13], entries[14]);
	mat3 m33 = mat3(entries[0], entries[1], entries[2], entries[4], entries[5], entries[6], entries[8], entries[9], entries[10]);

	float det = entries[0] * m00.GetDeterminate() - entries[4] * m01.GetDeterminate() + entries[8] * m02.GetDeterminate() - entries[12] * m03.GetDeterminate();
	if (det != 0)
	{
		mat4 adj = mat4(
			m00.GetDeterminate(), -m10.GetDeterminate(), m20.GetDeterminate(), -m30.GetDeterminate(),
			-m01.GetDeterminate(), m11.GetDeterminate(), -m21.GetDeterminate(), m31.GetDeterminate(),
			m02.GetDeterminate(), -m12.GetDeterminate(), m22.GetDeterminate(), -m32.GetDeterminate(),
			-m03.GetDeterminate(), m13.GetDeterminate(), -m23.GetDeterminate(), m33.GetDeterminate()
		);
		return adj * (1 / det);
	}
	return *this;
}

vec3 mat4::position() { return vec3(entries[12], entries[13], entries[14]); }
