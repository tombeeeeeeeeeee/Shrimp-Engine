#pragma once
#include <config.h>

const int MATERIAL_MAPCOUNT = 2;

enum MaterialMap
{
	color = 0,
	opacity = 1,
	//roughness
	//metalic
	//normal
	//emission
};


class Material
{
public:
	/// <summary>
	/// Creates new material
	/// </summary>
	/// <param name="name">name for the material</param>
	Material(const char* _name);
	Material(const char* _name, const char* filename);
	~Material();

	/// <summary>
	/// enables use of the Material
	/// </summary>
	void use();

	/// <summary>
	/// Enables use of one of the maps of the Material
	/// </summary>
	/// <param name="unit"> unit in the shader program that correlates to its map type</param>
	void use(int unit);

	/// <summary>
	/// Apply a texture to the material
	/// </summary>
	/// <param name="filename">texture file name</param>
	/// <param name="unit">type of map</param>
	void MapInitialise(const char* filename, int unit);

private:
	string name;
	unsigned int textures[MATERIAL_MAPCOUNT];
};

