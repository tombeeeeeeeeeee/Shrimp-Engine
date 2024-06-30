#pragma once
#include <config.h>
struct Plane
{
	glm::vec3 normal;
	float distance;
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};


struct CameraComponent
{
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 forward;

	float aspectRatio;
	float fov;
	float nearClip;
	float farClip;

	Frustum frustum;
};