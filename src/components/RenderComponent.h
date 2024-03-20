#pragma once
#include <config.h>

const int MATERIAL_MAPCOUNT = 2;

struct MeshAsset
{
	unsigned int VAO;
	unsigned int IBO;
	unsigned int vertexCount;
};

struct MaterialAsset
{
	unsigned int materials[MATERIAL_MAPCOUNT];
	unsigned int materialMask = 3;
	unsigned int shaderProgram = 0;
};

struct RenderComponent
{
	MaterialAsset* material;
	MeshAsset* mesh;
};