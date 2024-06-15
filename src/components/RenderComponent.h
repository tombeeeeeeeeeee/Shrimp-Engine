#pragma once
#include <config.h>

struct MeshAsset
{
	unsigned int VAO;
	unsigned int IBO;
	unsigned int triCount;
};

const int MATERIAL_MAPCOUNT = 3;

struct MaterialAsset
{
	unsigned int materials[MATERIAL_MAPCOUNT];
	unsigned int materialMask = 1;
	unsigned int shaderProgram = 0;
};

struct RenderComponent
{
	MaterialAsset* material;
	MeshAsset* mesh;
	glm::vec3 colour = {1,1,1};
};