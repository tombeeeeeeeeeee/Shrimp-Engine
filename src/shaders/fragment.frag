#version 330 core
in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;

out vec4 screenColor;

uniform sampler2D diffuse; //0
uniform sampler2D mask;		//1
uniform sampler2D normalMap;//2

uniform vec3 lightColor;    

const vec3 sunDirection = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
	vec3 baseColor = texture(diffuse, fragmentTexCoord).rgb;

	vec3 normalMapColor = texture(normalMap, fragmentTexCoord).rgb;

	float lightStrength = max(0.2, dot(fragmentNormal, sunDirection));
	screenColor = lightStrength * vec4(lightColor * baseColor, 1);

	float alpha = 1 - texture(mask, fragmentTexCoord).r;
	screenColor.a = alpha;
}

