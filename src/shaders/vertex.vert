#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;  

out vec2 fragmentTexCoord;
out vec3 fragmentPos;
out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentBitangent;


uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	fragmentTexCoord = vertexTexCoord;
	fragmentPos = (model * vec4(vertexPos, 0.0)).xyz;

	//Worse Normals Lower Tax
	fragmentTangent = (model * vec4(vertexTangent, 0.0)).xyz;
	fragmentNormal = (model * vec4(vertexNormal, 0.0)).xyz;
	fragmentBitangent = (model * vec4(vertexBitangent, 0.0)).xyz;


	//Better Normals Higher Tax
	fragmentTangent = normalize(fragmentTangent);
	fragmentNormal = normalize(fragmentNormal);
	fragmentTangent = normalize(fragmentTangent - dot(fragmentTangent, fragmentNormal) * fragmentNormal);
	fragmentBitangent = cross(fragmentNormal, fragmentTangent);
}