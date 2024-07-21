#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
in vec3 fragmentColour;

layout (location = 0) out vec4 screenColour;
layout (location = 1) out vec4 bloomColour;

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

	float brightness = dot(fragmentColour, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1)
	{
		bloomColour = vec4(fragmentColour, min(1 - brightness, 1));
	}
	else bloomColour = vec4(0.0, 0.0, 0.0, 1.0);
}
