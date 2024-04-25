#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;

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
	vec3 closestLightColour = vec3(0);
	float minLightDistance = 5000;

	int i = 0;
	while(i < lightPacketCount)
	{

		if(lightPackets[i].w == 1)
		{
			i += 1;
		}
		//Directional Light
		else if(lightPackets[i].w == 2)
		{
			i += 2;
		}
	
		//Point Light
		else if(lightPackets[i].w == 3)
		{
			vec3 lightDirection = lightPackets[i+1].xyz - fragmentPos;
			float distance = length(lightDirection);
			if(distance < minLightDistance)
			{
				minLightDistance = distance;
				closestLightColour = lightPackets[i].rgb;
			}
			i += 3;
		}
	
		//SpotLight
		else if(lightPackets[i].w == 4)
		{
			vec3 lightDirection = lightPackets[i+1].xyz - fragmentPos;
			float distance = length(lightDirection);

			if(distance < minLightDistance)
			{
				minLightDistance = distance;
				closestLightColour = lightPackets[i].rgb;
			}

			i += 4;
		}
	}

	screenColour = vec4(closestLightColour, 1);
}
