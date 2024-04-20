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

uniform int lightPacketCount;
uniform vec4 lightPackets[96]; // 32 lights

uniform	vec3 cameraPos;

void main()
{

	vec3 normalColour = texture(normalMap, fragmentTexCoord).rgb;
	normalColour = (normalColour.x == 0 && normalColour.y == 0 && normalColour.z == 0) ? vec3(0.5,0.5,1.0) : normalColour;
	normalColour = normalColour * 2.0 - 1.0;
	mat3 TBN = mat3(fragmentTangent, fragmentBitangent, fragmentNormal);
	vec3 trueNormal = normalize(TBN * normalColour);

	vec3 viewDirection = normalize(fragmentPos - cameraPos);

	vec3 diffuseColour = texture(diffuse, fragmentTexCoord).rgb;
	vec3 specularMaterialColour = texture(specular, fragmentTexCoord).rgb;
	float gloss = 16;
	
	vec3 lightColour;
	vec3 ambientLightColor;
	float specularLightIntensity;

	int i = 0;
	while(i < lightPacketCount)
	{
		//Ambient Light
		if(lightPackets[i].w == 1)
		{
			ambientLightColor += lightPackets[i].rgb;
			i += 1;
		}

		//Directional Light
		else if(lightPackets[i].w == 2)
		{
			float directionalLightIntensity = clamp(dot(trueNormal, lightPackets[i+1].xyz), 0, 1);
			vec3 halfwayRay = normalize(-lightPackets[i+1].xyz + viewDirection);
			specularLightIntensity += pow(clamp(dot(trueNormal, halfwayRay), 0, 1), gloss);
			lightColour += directionalLightIntensity * lightPackets[i].rgb;
			i += 2;
		}

		//Point Light
		else if(lightPackets[i].w == 3)
		{
			float distance = length(lightPackets[i+1].xyz - fragmentPos);
			float attenutation = 1 / (1 + lightPackets[i+1].y * distance + lightPackets[i+1].z * distance * distance);

			float directionalLightIntensity = clamp(dot(trueNormal, normalize(lightPackets[i+1].xyz - fragmentPos)), 0, 1);
			vec3 halfwayRay = normalize(lightPackets[i+1].xyz - fragmentPos + viewDirection);
			specularLightIntensity += pow(clamp(dot(trueNormal, halfwayRay), 0, 1), gloss);

			lightColour += directionalLightIntensity * attenutation * lightPackets[i].rgb;
			i += 3;
		}

		//SpotLight TODO
		else if(lightPackets[i].w == 4)
		{
			i += 4;
		}
	}
	
	vec3 D = diffuseColour * lightColour;
	vec3 S = specularMaterialColour.rgb * specularLightIntensity;
	vec3 A = ambientLightColor * diffuseColour;
	screenColour = vec4(D + S + A, 0);

	screenColour = vec4(pow(screenColour.rgb, vec3(1.0/2.2)), screenColour.a);
}

