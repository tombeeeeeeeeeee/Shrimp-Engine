#include "LinearAlgebra.h"

vec3 mat4::right() { return vec3(entries[1], entries[2], entries[2]); }
vec3 mat4::up() { return vec3(entries[4], entries[5], entries[6]); }
vec3 mat4::forward() { return vec3(entries[8], entries[9], entries[10]); }
vec3 mat4::position() { return vec3(entries[12], entries[13], entries[14]); }

mat4 mat4::inverse()
{
	return mat4(
		this->right().x, this->up().x, -(this->forward().x), 0,
		this->right().y, this->up().y, -(this->forward().y), 0,
		this->right().z, this->up().z, -(this->forward().z), 0,
		-(this->right().dot(position())), -(this->up().dot(position())), this->forward().dot(position()), 1
	);
}

vec4 vec4::normal()
{
	float mag = this->magnitude;
	return vec4(this->x / mag, this->y / mag, this->z / mag, this->w / mag);
}

vec3 vec3::cross(vec3 b)
{
	return vec3(
		this->y * b.z - b.y * this->z,
		this->z * b.x - b.z * this->x,
		this->x * b.y - b.x * this->y
	);
}


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

mat4 ViewMatrix(vec3 from, vec3 to, vec3 up)
{

	vec3 f = normalize(to - from);
	vec3 r = normalize(cross(f, up));
	vec3 u = normalize(cross(r, f));

	return mat4(
		r.x, u.x, -f.x, 0,
		r.y, u.y, -f.y, 0,
		r.z, u.z, -f.z, 0,
		-(r.dot(from)), -(u.dot(from)), f.dot(from), 1
	);
}

ostream& operator<<(ostream& os, vec3 const& vec)
{
	return os << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }" << endl;
}

ostream& operator<<(ostream& os, mat4 const& mat)
{
	return 
	os << "| " << mat.entries[0] << ", " << mat.entries[4] << ", " << mat.entries[8] << ", " << mat.entries[12] << " |" << endl
	   << "| " << mat.entries[1] << ", " << mat.entries[5] << ", " << mat.entries[9] << ", " << mat.entries[13] << " |" << endl
	   << "| " << mat.entries[2] << ", " << mat.entries[6] << ", " << mat.entries[10] << ", " << mat.entries[14] << " |" << endl
	   << "| " << mat.entries[3] << ", " << mat.entries[7] << ", " << mat.entries[11] << ", " << mat.entries[15] << " |" << endl;
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
		0, 0, -(n+f)/(n-f), -1,
		0, 0, 2 * n * f / (n-f), 0
	);
}

vec4 normalize(vec4 vec)
{
	return vec.normal();
}

vec3 normalize(vec3 vec)
{
	return vec.normal();
}

vec3 cross(vec3 a, vec3 b)
{
	return a.cross(b);
}

