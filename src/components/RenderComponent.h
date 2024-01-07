#pragma once
#include <config.h>

const int MATERIAL_MAPCOUNT = 2;


struct RenderComponent
{
	unsigned int materials[MATERIAL_MAPCOUNT];
	unsigned int materialMask = 3;
	unsigned int mesh;
};