#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;

out vec2 fragmentTexCoord;
out vec3 fragmentPos;

uniform mat4 model; //local transform
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	fragmentTexCoord = vertexTexCoord;
	//fragmentNormal = (model * vec4(vertexNormal, 0.0)).xyz;
	fragmentPos = (model * vec4(vertexPos, 0.0)).xyz;
}