#include "Material.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>


Material::Material(const char* _name)
{
	name = string::basic_string(_name);
}


Material::Material(const char* _name ,const char* filename)
{
	name = string::basic_string(_name);
	MapInitialise(filename, color);
}

Material::~Material()
{
	for (unsigned int texture : textures)
		if(texture != 0) glDeleteTextures(1, &texture);
}

void Material::use()
{
	for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
	{
		if (textures[i] != 0)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
	}
}

void Material::use(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textures[unit]);
}

void Material::MapInitialise(const char* filename, int unit)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	
	/* Debuging Code
	cout << width << " x " << height;
	ColouredOutput(" image was loaded from: " + string::basic_string(filename), green);
	//*/

	//make texture
	glCreateTextures(GL_TEXTURE_2D, 1, &textures[unit]);
	glBindTexture(GL_TEXTURE_2D, textures[unit]);

	//upload texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//free data
	stbi_image_free(data);

	//configure sampler
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
