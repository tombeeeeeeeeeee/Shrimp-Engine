#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;
in vec3 fragmentColour;

out vec4 screenColour;

uniform sampler2D diffuse;			//0
uniform sampler2D specular;			//1
uniform sampler2D normalMap;		//2
uniform samplerCube irradianceMap;	//3
uniform samplerCube prefilterMap;	//4
uniform sampler2D brdfLUT;			//5

uniform int lightPacketCount;
uniform vec4 lightPackets[64];

uniform	vec3 cameraPos;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
	vec4 PBRpacked = texture(specular, fragmentTexCoord);
	float metallic = PBRpacked.r;
	float roughness = PBRpacked.g; 
	float ao = PBRpacked.b;

	vec3 normalColour = texture(normalMap, fragmentTexCoord).rgb;
	normalColour = (normalColour.x == 0 && normalColour.y == 0 && normalColour.z == 0) ? vec3(0.5,0.5,1.0) : normalColour;
	normalColour = normalColour * 2.0 - 1.0;
	mat3 TBN = mat3(normalize(fragmentTangent), normalize(fragmentBitangent), normalize(fragmentNormal));
	vec3 trueNormal = normalize(TBN * normalColour);
	
	vec3 viewDirection = normalize(cameraPos - fragmentPos);
	vec3 reflected = reflect(-viewDirection, trueNormal); 
	vec3 albedo = texture(diffuse, fragmentTexCoord).rgb;

	vec3 Lo = vec3(0.0);
	vec3 ambientLightColour = vec3(0.0);

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	int i = 0;
	while(i < lightPacketCount)
	{
		int packetStep = 0;
		//Set Up for light
		vec3 lightDirection = vec3(0.0);
		vec3 halfwayRay = vec3(0.0);
		vec3 radiance = vec3(0.0);
	
		//Directional Light
		if(lightPackets[i].w == 2)
		{
			lightDirection = normalize(lightPackets[i+1].xyz);
			halfwayRay = normalize(lightDirection + viewDirection);
			radiance = lightPackets[i].rgb;

			packetStep = 2;
		}
	
		//Point Light
		else if(lightPackets[i].w == 3)
		{
			lightDirection = lightPackets[i+1].xyz - fragmentPos;
			float distance = length(lightDirection);
			lightDirection = normalize(lightDirection);

			halfwayRay = normalize(lightDirection + viewDirection);
			float attenuation  = 1 / (1 + lightPackets[i+2].y * distance + lightPackets[i+2].z * distance * distance);
			radiance = lightPackets[i].rgb * attenuation;

			packetStep = 3;
		}
	
		//SpotLight
		else if(lightPackets[i].w == 4)
		{
			lightDirection = lightPackets[i+1].xyz - fragmentPos;
			float distance = length(lightDirection);
			lightDirection = normalize(lightDirection);

			halfwayRay = normalize(lightDirection + viewDirection);
			float attenuation  = 1 / (1 + lightPackets[i+3].y * distance + lightPackets[i+3].z * distance * distance);
			float theta = dot(lightDirection, -normalize(lightPackets[i+2].xyz));
			float epsilon = lightPackets[i+3].x - lightPackets[i+3].w;
			float intensity = clamp((theta - lightPackets[i+3].w) / epsilon, 0.0, 1.0);
			radiance = lightPackets[i].rgb * attenuation * intensity;

			packetStep = 4;
		}

		else if(lightPackets[i].w == 1)
		{	
			ambientLightColour += lightPackets[i].rgb;
			packetStep = 1;
		}

		float normalDist = DistributionGGX(trueNormal, halfwayRay, roughness);
		vec3 F = fresnelSchlick(max(dot(halfwayRay, viewDirection), 0.0), F0);
		float G = GeometrySmith(trueNormal, viewDirection, lightDirection, roughness);

		vec3 numerator = normalDist * F * G;
		float denominator = 4.0 * max(dot(trueNormal, viewDirection), 0.0) * max(dot(trueNormal, lightDirection), 0.0) + 0.0001;
		vec3 BRDF = numerator / denominator;

		vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

		float NdotL = max(dot(trueNormal, lightDirection), 0.0);        
		Lo += (kD * albedo / PI + BRDF) * radiance * NdotL;



		i += packetStep;
	}

	vec3 kS = fresnelSchlickRoughness(max(dot(trueNormal, viewDirection), 0.0), F0, roughness);
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;	  
    vec3 irradiance = fragmentColour * texture(irradianceMap, trueNormal).rgb;

    vec3 diffuse = irradiance * albedo;
	vec3 additionalAmbient = (kD * ambientLightColour) * ao;
    vec3 prefilteredColor = textureLod(prefilterMap, reflected,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(trueNormal, viewDirection), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

	screenColour = vec4(Lo + ambient + additionalAmbient, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

