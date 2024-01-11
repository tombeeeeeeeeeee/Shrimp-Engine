#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec2 fragmentTexCoord;
out mat3 fragmentTBN;

in DATA
{
	vec2 geoTexCoord;
	vec3 geoNormal;
    mat4 model;
} data_in[];


void main()
{
	// Edges of the triangle
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    
    // Lengths of UV differences
    vec2 deltaUV0 = data_in[1].geoTexCoord - data_in[0].geoTexCoord;
    vec2 deltaUV1 = data_in[2].geoTexCoord - data_in[0].geoTexCoord;

    // one over the determinant
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    TBN = transpose(TBN);

    fragmentTexCoord = data_in[0].geoTexCoord;
    fragmentTBN = TBN;
    EmitVertex();
    
    fragmentTexCoord = data_in[1].geoTexCoord;
    fragmentTBN = TBN;
    EmitVertex();

    fragmentTexCoord = data_in[2].geoTexCoord;
    fragmentTBN = TBN;
    EmitVertex();

    EndPrimitive();
}