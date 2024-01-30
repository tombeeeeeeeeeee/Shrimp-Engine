#version 330 core

in DATA {
    vec3 fragmentPos;
    vec2 fragmentTexCoord;
    vec3 fragmentNormal;
    //vec3 TangentLightPos;
    //vec3 TangentViewPos;
    //vec3 TangentFragPos;
} data_in;

out vec4 screenColor;

uniform sampler2D material; //0
uniform sampler2D mask;		//1
uniform sampler2D normalMap;//2
uniform vec3 lightColor;    

const vec3 sunDirection = normalize(vec3(0, 1, 1));

void main()
{
	vec3 baseColor = texture(material, data_in.fragmentTexCoord).rgb;

	//vec3 normalMapColor = texture(normalMap, data_in.fragmentTexCoord).rgb;
	//vec3 worldNormal = normalize(TBN * (normalMapColor * 2.0 - 1.0));

	float lightStrength = max(0.2, dot(data_in.fragmentNormal, sunDirection));
	screenColor = lightStrength * vec4(lightColor * baseColor, 1);

	float alpha = 1 - texture(mask, data_in.fragmentTexCoord).r;
	screenColor.a = alpha;
}

