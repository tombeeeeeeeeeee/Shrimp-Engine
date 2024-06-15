#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
in vec3 fragmentColour;

out vec4 screenColour;

uniform sampler2D diffuse;  //0
uniform sampler2D specular; //1
uniform sampler2D normalMap; //2
uniform samplerCube irradianceMap;

uniform int lightPacketCount;
uniform vec4 lightPackets[96];

uniform	vec3 cameraPos;

void main()
{
	screenColour = vec4(fragmentColour, 1);
}
