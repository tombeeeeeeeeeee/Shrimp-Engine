#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;

out vec4 screenColor;

uniform sampler2D diffuse; //0
uniform sampler2D mask;		//1
uniform sampler2D normalMap;//2

uniform vec3 directionalLightColor;    
uniform vec3 directionalLightDirection;
uniform vec3 ambientLightColor;
uniform float ambientLightStrength;

void main()
{
	vec3 baseColor = texture(diffuse, fragmentTexCoord).rgb;

	vec3 normalMapColor = texture(normalMap, fragmentTexCoord).rgb;

	float directionalLightStrength = max(0, dot(normalize(fragmentNormal), directionalLightDirection));
	screenColor = directionalLightStrength * vec4(directionalLightColor * baseColor, 1) + ambientLightStrength * vec4(ambientLightColor * baseColor, 0);

	float alpha = 1 - texture(mask, fragmentTexCoord).r;
	screenColor.a = alpha;
}

