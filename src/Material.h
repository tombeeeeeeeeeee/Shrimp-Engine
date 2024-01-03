#pragma once
#include <config.h>

class Material
{
public:
	Material(const char* filename);
	~Material();
	void use(int unit);

private:
	string name;
	unsigned int textures[2];
};

