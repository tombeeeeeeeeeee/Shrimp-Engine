#include "RenderComponent.h"

Vertex::Vertex(vec3 pos, vec3 nor, vec2 tex, vec3 tan, vec3 bit)
{
	position = pos;
	normal = nor;
	texCoord = tex;
	tangent = tan;
	biTangent = bit;
}
