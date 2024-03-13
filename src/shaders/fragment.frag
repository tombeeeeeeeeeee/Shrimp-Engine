#version 330 core

in vec2 fragmentTexCoord;
in vec3 fragmentPos;
in vec3 fragmentNormal;

out vec4 screenColor;

uniform sampler2D material; //0
uniform sampler2D mask;		//1
uniform sampler2D normalMap;//2

const vec3 sunDirection = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
	vec3 baseColor = texture(material, fragmentTexCoord).rgb;

	vec3 normalMapColor = texture(normalMap, fragmentTexCoord).rgb;
	vec3 worldNormal = normalize(fragmentNormal * (normalMapColor * 2.0 - 1.0));

	float lightStrength = max(0.2, dot(worldNormal, sunDirection));
	screenColor = lightStrength * vec4(baseColor, 1);

	float alpha = 1 - texture(mask, fragmentTexCoord).r;
	screenColor.a = alpha;

	vec3 fragmentPosCol = normalize(fragmentPos);
	fragmentPosCol *= 0.2;

	screenColor.x += fragmentPosCol.x;
	screenColor.y += fragmentPosCol.y;
	screenColor.z += fragmentPosCol.z;
}

