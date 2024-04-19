#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentTangent;
in vec3 fragmentBitangent;

out vec4 screenColor;

uniform sampler2D diffuse;  //0
uniform sampler2D specular; //1
uniform sampler2D normalMap;//2

uniform vec3 directionalLightColor;    
uniform vec3 directionalLightDirection;
uniform vec3 ambientLightColor;
uniform float ambientLightStrength;
uniform vec3 cameraPos;

void main()
{

	vec3 normalColour = texture(normalMap, fragmentTexCoord).rgb;
	normalColour = normalColour * 2.0 - 1.0;
	mat3 TBN = mat3(fragmentTangent, fragmentBitangent, fragmentNormal);
	vec3 trueNormal = normalize(TBN * normalColour);

	vec3 viewDirection = normalize(fragmentPos - cameraPos);

	vec3 diffuseColour = texture(diffuse, fragmentTexCoord).rgb;
	vec4 specularMaterialColour = texture(specular, fragmentTexCoord).rgba;
	float gloss = specularMaterialColour.a * 8;

	//Needs to be refactored to be Per Light
	float directionalLightIntensity = clamp(dot(trueNormal, -directionalLightDirection), 0, 1);
	vec3 reflectedRay = reflect(-directionalLightDirection, trueNormal);
	float specularLightIntensity = pow(clamp(dot(viewDirection, reflectedRay), 0, 1), gloss);
	
	vec3 D = diffuseColour * directionalLightIntensity * directionalLightColor;
	vec3 S = sign(gloss) *specularMaterialColour.rgb * specularLightIntensity;
	vec3 A = ambientLightStrength * ambientLightColor * diffuseColour;
	screenColor = vec4(D + S + A, 0);

	screenColor = vec4(pow(screenColor.rgb, vec3(1.0/2.2)), screenColor.a);
}

