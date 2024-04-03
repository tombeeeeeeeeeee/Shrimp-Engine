#pragma once
#include <config.h>


struct Vertex
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 biTangent;

	Vertex(vec3 pos, vec3 nor = { 0.f, 0.f, 0.f }, vec2 tex = { 0.f, 0.f }, vec3 tan = { 0.f, 0.f, 0.f }, vec3 bit = { 0.f, 0.f, 0.f });
	Vertex() {};
};

struct MeshAsset
{
	unsigned int VAO;
	unsigned int IBO;
	unsigned int vertexCount;
};

const int MATERIAL_MAPCOUNT = 2;

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
};