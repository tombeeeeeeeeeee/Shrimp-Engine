#version 330 core

in vec2 fragmentTexCoord;
in vec3 fragmentPos;

out vec4 screenColor;

uniform sampler2D material; //0
uniform sampler2D mask;		//1
uniform sampler2D normalMap;//2

const vec3 sunDirection = normalize(vec3(-1.0, 1.0, 1.0));

void main()
{
	vec3 baseColor = texture(material, fragmentTexCoord).rgb;

	vec3 dp1 = dFdx(fragmentPos);
	vec3 dp2 = dFdy(fragmentPos);
	vec2 duv1 = dFdx(fragmentTexCoord);
	vec2 duv2 = dFdy(fragmentTexCoord);

	vec3 tangent = normalize(dp1 * duv2.y - dp2 * duv1.y);
    vec3 bitangent = normalize(-dp1 * duv2.x + dp2 * duv1.x);
    vec3 normal = normalize(cross(tangent, bitangent));

	mat3 TBN = mat3(tangent, bitangent, normal);

	vec3 normalMapColor = texture(normalMap, fragmentTexCoord).rgb;
	vec3 worldNormal = normalize(TBN * (normalMapColor * 2.0 - 1.0));

	float lightStrength = max(0.2, dot(worldNormal, sunDirection));
	screenColor = lightStrength * vec4(baseColor, 1);

	float alpha = 1 - texture(mask, fragmentTexCoord).r;
	screenColor.a = alpha;
}

