#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;
//layout (location = 3) in vec3 vertexTangent;
//layout (location = 4) in vec3 vertexBitangent;  

out vec2 fragmentTexCoord;
out vec3 fragmentPos;
out vec3 fragmentNormal;

uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

//uniform vec3 lightPos;
//uniform vec3 viewPos;

void main()
{

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	fragmentTexCoord = vertexTexCoord;
	fragmentNormal = normalize((model * vec4(vertexNormal, 0.0)).xyz);
	fragmentPos = (model * vec4(vertexPos, 0.0)).xyz;
}