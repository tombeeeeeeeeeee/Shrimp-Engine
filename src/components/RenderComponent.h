#pragma once
#include <config.h>

const int MATERIAL_MAPCOUNT = 2;


struct RenderComponent
{
	unsigned int materials[MATERIAL_MAPCOUNT];
	unsigned int materialMask;
	unsigned int mesh;
};