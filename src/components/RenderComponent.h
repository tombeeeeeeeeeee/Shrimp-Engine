#pragma once
#include <config.h>

const int MATERIAL_MAPCOUNT = 3;


struct RenderComponent
{
	unsigned int materials[MATERIAL_MAPCOUNT];
	unsigned int materialMask = 7;
	unsigned int VAO;
	unsigned int vertexCount;
};