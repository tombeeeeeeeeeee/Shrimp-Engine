#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;

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

	//vec3 normalMapColor = texture(normalMap, fragmentTexCoord).rgb;

	vec3 trueNormal = normalize(fragmentNormal);

	vec3 viewDirection = normalize(fragmentPos - cameraPos);

	vec3 diffuseColour = texture(diffuse, fragmentTexCoord).rgb;
	vec4 specularMaterialColour = texture(specular, fragmentTexCoord).rgba;
	float gloss = length(specularMaterialColour) * 10;

	//Needs to be refactored to be Per Light
	float directionalLightIntensity = clamp(dot(trueNormal, -directionalLightDirection), 0, 1);
	vec3 reflectedRay = reflect(-directionalLightDirection, trueNormal);
	float specularLightIntensity = pow(clamp(dot(viewDirection, reflectedRay), 0, 1), gloss);

	//screenColor = directionalLightIntensity * vec4(directionalLightColor * diffuseColour, 1) + ambientLightStrength * vec4(ambientLightColor * diffuseColour, 0);
	vec3 D = diffuseColour * directionalLightIntensity * directionalLightColor;
	vec3 S = specularMaterialColour.rgb * specularLightIntensity;
	vec3 A = ambientLightStrength * ambientLightColor * diffuseColour;
	screenColor = vec4(D + S + A, 0);
}

