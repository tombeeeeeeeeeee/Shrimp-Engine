#include "Material.h"

Material::Material(const char* filename)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	
	cout << width << " x " << height;
	ColouredOutput(" image was loaded from: " + string::basic_string(filename), green);

	//make texture
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

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

Material::~Material()
{
	for (unsigned int texture : textures)
		if(texture != 0) glDeleteTextures(1, &texture);
}

void Material::use(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}
