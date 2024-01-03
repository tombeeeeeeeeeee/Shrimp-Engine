#pragma once
#include "config.h"

class TriangleMesh {
public:
	TriangleMesh();
	void draw();
	~TriangleMesh();

private:
	unsigned int EBO, VBO, VAO, vertex_count;
};