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
uniform vec4 lightPackets[96];

uniform	vec3 cameraPos;

void main()
{
	vec4 PBRpacked = texture(diffuse, fragmentTexCoord);
	float metallic = PBRpacked.r;
	float roughness = PBRpacked.g;
	float ao = PBRpacked.b;

	vec3 normalColour = texture(normalMap, fragmentTexCoord).rgb;
	normalColour = (normalColour.x == 0 && normalColour.y == 0 && normalColour.z == 0) ? vec3(0.5,0.5,1.0) : normalColour;
	normalColour = normalColour * 2.0 - 1.0;
	mat3 TBN = mat3(normalize(fragmentTangent), normalize(fragmentBitangent), normalize(fragmentNormal));
	vec3 trueNormal = normalize(TBN * normalColour);
	
	vec3 viewDirection = normalize(fragmentPos - cameraPos);
	
	vec3 albedo = texture(diffuse, fragmentTexCoord).rgb;

	vec3 Lo = vec3(0.0);
	vec3 ambientLightColor; //TO DO ON CPU



	//int i = 0;
	//while(i < lightPacketCount)
	//{
	//	//Ambient Light
	//	if(lightPackets[i].w == 1)
	//	{
	//		ambientLightColor += lightPackets[i].rgb;
	//		i += 1;
	//	}
	//
	//	//Directional Light
	//	else if(lightPackets[i].w == 2)
	//	{
	//		float directionalLightIntensity = clamp(dot(trueNormal, lightPackets[i+1].xyz), 0, 1);
	//		vec3 halfwayRay = -normalize(-lightPackets[i+1].xyz + viewDirection);
	//		specularLightIntensity += pow(clamp(dot(trueNormal, halfwayRay), 0, 1), gloss);
	//		lightColour += directionalLightIntensity * lightPackets[i].rgb;
	//		i += 2;
	//	}
	//
	//	//Point Light
	//	else if(lightPackets[i].w == 3)
	//	{
	//		vec3 direction = fragmentPos - lightPackets[i+1].xyz;
	//		float distance = length(direction);
	//		direction = normalize(direction);
	//
	//		float attenuation  = 1 / (1 + lightPackets[i+2].y * distance + lightPackets[i+2].z * distance * distance);
	//
	//		float directionalLightIntensity = clamp(dot(trueNormal, -direction), 0, 1);
	//		vec3 halfwayRay = -normalize(direction - viewDirection);
	//		specularLightIntensity += pow(clamp(dot(trueNormal, halfwayRay), 0, 1), gloss) * attenuation;
	//
	//		lightColour += directionalLightIntensity * attenuation  * lightPackets[i].rgb;
	//		i += 3;
	//	}
	//
	//	//SpotLight
	//	else if(lightPackets[i].w == 4)
	//	{
	//		vec3 direction = fragmentPos - lightPackets[i+1].xyz;
	//		float distance = length(direction);
	//		direction = normalize(direction);
	//
	//		float theta = dot(direction, normalize(lightPackets[i+2].xyz));
	//		float epsilon = lightPackets[i+3].x - lightPackets[i+3].w;
	//		float intensity = clamp((theta - lightPackets[i+3].w) / epsilon, 0.0, 1.0);
	//
	//		float attenuation  = 1 / (1 + lightPackets[i+3].y * distance + lightPackets[i+3].z * distance * distance);
	//
	//		float directionalLightIntensity = clamp(dot(trueNormal, -direction), 0, 1);
	//		vec3 halfwayRay = -normalize(direction - viewDirection);
	//		specularLightIntensity += pow(clamp(dot(trueNormal, halfwayRay), 0, 1), gloss) * attenuation * intensity;
	//
	//		lightColour += directionalLightIntensity * attenuation  * lightPackets[i].rgb * intensity;
	//
	//		i += 4;
	//	}
	//}
	//
	//vec3 D = diffuseColour * lightColour;
	//vec3 S = specularMaterialColour.rgb * specularLightIntensity;
	//vec3 A = ambientLightColor * diffuseColour;
	//screenColour = vec4(D + S + A, 0);
	//
	//screenColour = vec4(pow(screenColour.rgb, vec3(1.0/2.2)), screenColour.a);
}

