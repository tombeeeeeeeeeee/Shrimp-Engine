#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;
//layout (location = 3) in vec3 vertexTangent;
//layout (location = 4) in vec3 vertexBitangent;  

out DATA {
    vec3 fragmentPos;
    vec2 fragmentTexCoord;
    vec3 fragmentNormal;
    //vec3 TangentLightPos;
    //vec3 TangentViewPos;
    //vec3 TangentFragPos;
} data_out;

uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

//uniform vec3 lightPos;
//uniform vec3 viewPos;

void main()
{

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	data_out.fragmentTexCoord = vertexTexCoord;
	data_out.fragmentNormal = (model * vec4(vertexNormal, 0.0)).xyz;

    //vec3 T = normalize(vec3(model * vec4(vertexTangent,   0.0)));
    //vec3 B = normalize(vec3(model * vec4(vertexBitangent, 0.0)));
    //vec3 N = normalize(vec3(model * vec4(vertexNormal,    0.0)));
    //
    //mat3 TBN = transpose(mat3(T, B, N));
    //frag_data.TangentLightPos = TBN * lightPos;
    //frag_data.TangentViewPos  = TBN * viewPos;
    //frag_data.TangentFragPos  = TBN * vec3(model * vec4(aPos, 1.0));

}